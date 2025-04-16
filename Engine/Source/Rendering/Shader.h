#pragma once
#include "Renderer.h"
#include "Runtime/Filesystem.h"
#include "Runtime/ShaderSource.h"
#include "Runtime/LogCategory.h"
#include "Math/LinearAlgebra.h"
#include "Containers/String.h"
#include "Runtime/Asset.h"
#include "Components/Rendering/AmbientLight.h"
#include "Components/Rendering/PointLight.h"
#include "Rendering/Renderer.h"


namespace Nova
{
    class DirectionalLight;
}

NOVA_DECLARE_LOG_CATEGORY_STATIC(Shader, "SHADER");

namespace Nova
{
    class Texture2D;

    class Shader : public Asset
    {
    public:
        ~Shader() override = default;
        String GetAssetType() const override;
        virtual bool Reload()
        {
            Bind();
            Delete();
            LoadSource(m_Filepath);
            bool Result = Compile();
            if(!Result) return false;

            Result = Link();
            if(!Result) return false;

            Result = Validate();
            if(!Result) return false;
            return true;
        }
        virtual bool Compile() = 0;
        virtual bool Link() = 0;
        virtual bool Validate() = 0;
        virtual bool Bind() = 0;
        virtual void Delete() = 0;

        
        void SetLight(const String& Name, const LightComponent* Light);
        virtual void SetDirectionalLight(const String& Name, const DirectionalLight* DirLight){}
        virtual void SetPointLight(const String& Name, const PointLight* PointLight){}
        virtual void SetAmbientLight(const String& Name, const AmbientLight* AmbientLight){}
        
        virtual void SetUniformFloat(const String& Name, f32 Value){}
        virtual void SetUniformFloat2(const String& Name, const Vector2& Value){}
        virtual void SetUniformFloat3(const String& Name, const Vector3& Value){}
        virtual void SetUniformFloat4(const String& Name, const Vector4& Value){}
        virtual void SetUniformMat4(const String& Name, const Matrix4& Value){}
        virtual void SetUniformInt(const String& Name, i32 Value){}
        virtual void SetUniformTexture(const String& Name, Texture2D* Texture){}
        virtual void SetUniformMat2(const String& Name, const Matrix2& Value){}
        virtual void SetUniformMat3(const String& Name, const Matrix3& Value){}

        virtual f32 GetUniformFloat(const String& Name){ return 0.0f; }
        virtual Vector2 GetUniformFloat2(const String& Name){ return Vector2::Zero; }
        virtual Vector3 GetUniformFloat3(const String& Name){ return Vector3::Zero; }
        virtual Vector4 GetUniformFloat4(const String& Name){ return Vector4::Zero; }
        virtual Matrix4 GetUniformMat4(const String& Name){ return Matrix4::Identity; }
        virtual i32 GetUniformInt(const String& Name){ return 0;}
        virtual i32 GetUniformLocation(const String& Name) = 0;
        String GetFilename() const;

        const ShaderSource& GetSource() const;
    protected:
        explicit Shader(Renderer* Renderer, const String& Name, Path Filepath);
        Renderer* m_Renderer = nullptr;
        Path m_Filepath;
        ShaderSource m_Source;

        bool Compiled{false};
        bool Linked{false};
        bool Validated{false};

        void LoadSource(Path Filepath);
        void SplitSources(const String& Source);
        bool Preprocess(String& Source);
    };


    
}
