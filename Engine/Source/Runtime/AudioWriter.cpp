#include "AudioWriter.h"
#include "Random.h"

#include <vorbis/vorbisenc.h>
#include <drlibs/drmp3.h>
#include <drlibs/drwav.h>


namespace Nova
{
    bool WavWriter::Write(BufferView<u8> Samples, Stream& OutStream)
    {
        drwav Wav;
        drwav_data_format Format;
        Format.format = DR_WAVE_FORMAT_PCM;
        Format.channels = m_InFormat.Channels;
        Format.container = drwav_container_riff;
        Format.sampleRate = m_InFormat.SampleRate;
        Format.bitsPerSample = m_InFormat.GetBitsPerSample();

        const auto OnWrite = [](void* pUserData, const void* pData, size_t bytesToWrite) -> size_t
        {
            Stream& UserStream = *static_cast<Stream*>(pUserData);
            return UserStream.Write(pData, bytesToWrite);
        };

        const auto OnSeek = [](void* pUserData, int offset, drwav_seek_origin origin) -> drwav_bool32
        {
            Stream& UserStream = *static_cast<Stream*>(pUserData);
            return UserStream.Seek((Seek)origin, offset);
        };
        
        if (!drwav_init_write(&Wav, &Format, OnWrite, OnSeek, &OutStream, nullptr))
            return false;

        i64 Frames = (i64)Samples.Count() / m_InFormat.GetBitsPerSample();
        drwav_write_pcm_frames(&Wav, Frames, Samples.Data());
        drwav_uninit(&Wav);
        return true;
    }

    bool OggWriter::Write(BufferView<u8> Samples, Stream& OutStream)
    {
        vorbis_info VorbisInfo;
        vorbis_info_init(&VorbisInfo);
        if (vorbis_encode_init_vbr(&VorbisInfo,(long)m_InFormat.Channels, (long)m_InFormat.SampleRate, 0.8f))
        {
            vorbis_info_clear(&VorbisInfo);
            return false;
        }

        vorbis_comment VorbisComment;
        vorbis_comment_init(&VorbisComment);
        vorbis_comment_add_tag(&VorbisComment, "ENCODER", "Nova Engine OGG Vorbis");

        vorbis_dsp_state VorbisDspState;
        if (vorbis_analysis_init(&VorbisDspState, &VorbisInfo))
        {
            vorbis_info_clear(&VorbisInfo);
            vorbis_comment_clear(&VorbisComment);
            return false;
        }

        vorbis_block VorbisBlock;
        vorbis_block_init(&VorbisDspState, &VorbisBlock);

        ogg_stream_state StreamState;
        ogg_stream_init(&StreamState, Random::Integer(I32_MIN, I32_MAX));

        
        ogg_packet HeaderPacket, headerComm, headerCode;
        vorbis_analysis_headerout(&VorbisDspState, &VorbisComment, &HeaderPacket, &headerComm, &headerCode);
        ogg_stream_packetin(&StreamState, &HeaderPacket);
        ogg_stream_packetin(&StreamState, &headerComm);
        ogg_stream_packetin(&StreamState, &headerCode);

        ogg_page Page;
        while (ogg_stream_flush(&StreamState, &Page))
        {
            OutStream.Write(Page.header, Page.header_len);
            OutStream.Write(Page.body, Page.body_len);
        }
        
        const u32 BitsPerSample = m_InFormat.GetBitsPerSample();
        const size_t FramesCount = Samples.Count() / BitsPerSample;
        const size_t SamplesPerChannel = FramesCount / m_InFormat.Channels;
        size_t Position = 0;

        while (Position < SamplesPerChannel)
        {
            float** Buffer = vorbis_analysis_buffer(&VorbisDspState, 1024);
            i32 Count = (i32)Math::Min(SamplesPerChannel - Position, 1024ULL);

            for (i32 i = 0; i < Count; ++i)
            {
                for (i32 Channel = 0; Channel < (i32)m_InFormat.Channels; ++Channel)
                {
                    Buffer[Channel][i] = (f32)Samples[Position * m_InFormat.Channels * BitsPerSample + Channel] / (f32)I16_MAX;
                }
                Position++;
            }

            vorbis_analysis_wrote(&VorbisDspState, Count);
            
            while (vorbis_analysis_blockout(&VorbisDspState, &VorbisBlock) == 1)
            {
                vorbis_analysis(&VorbisBlock, nullptr);
                vorbis_bitrate_addblock(&VorbisBlock);

                ogg_packet Packet;
                while (vorbis_bitrate_flushpacket(&VorbisDspState, &Packet))
                {
                    ogg_stream_packetin(&StreamState, &Packet);

                    while (ogg_stream_pageout(&StreamState, &Page))
                    {
                        OutStream.Write(Page.header, Page.header_len);
                        OutStream.Write(Page.body, Page.body_len);
                    }
                }
            }
        }
        
        vorbis_analysis_wrote(&VorbisDspState, 0);
        while (vorbis_analysis_blockout(&VorbisDspState, &VorbisBlock) == 1)
        {
            vorbis_analysis(&VorbisBlock, nullptr);
            vorbis_bitrate_addblock(&VorbisBlock);

            ogg_packet Packet;
            while (vorbis_bitrate_flushpacket(&VorbisDspState, &Packet))
            {
                ogg_stream_packetin(&StreamState, &Packet);

                while (ogg_stream_pageout(&StreamState, &Page))
                {
                    OutStream.Write(Page.header, Page.header_len);
                    OutStream.Write(Page.body, Page.body_len);
                }
            }
        }
        
        ogg_stream_clear(&StreamState);
        vorbis_block_clear(&VorbisBlock);
        vorbis_dsp_clear(&VorbisDspState);
        vorbis_comment_clear(&VorbisComment);
        vorbis_info_clear(&VorbisInfo);
        return true;
    }
    

    bool Mp3Writer::Write(BufferView<u8> Samples, Stream& OutStream)
    {
        return false;
    }
}

