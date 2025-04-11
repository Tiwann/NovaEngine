#include "ModelRenderer.h"

#include "DirectionalLight.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Runtime/Model.h"
#include "Runtime/PopupMessage.h"
#include "Rendering/VertexArray.h"
#include "Editor/EditorGUI.h"
#include "ResourceManager/ShaderManager.h"
#include "Runtime/Application.h"
#include "Rendering/Renderer.h"
#include "Rendering/VertexBuffer.h"


namespace Nova
{
    ModelRenderer::ModelRenderer(Entity* Owner) : Component(Owner, "Model Component")
    {
    }

    void ModelRenderer::OnInit()
    {
        Component::OnInit();
        m_Model = new Model("TransientModel");
        ShaderManager* ShaderManager = g_Application->GetShaderManager();
        m_Shader = ShaderManager->Retrieve("BlinnPhong");
    }

    void ModelRenderer::OnDestroy()
    {
        Component::OnDestroy();
        delete m_Model;
    }

    void ModelRenderer::OnRender(Renderer* Renderer)
    {
        Component::OnRender(Renderer);
        if (m_Model->GetMeshes().Count() <= 0) return;
        
        const Array<LightComponent*> AllLights = g_Application->GetScene()->GetAllComponents<LightComponent>();
        
        const Camera* Camera = Renderer->GetCurrentCamera();
        const Transform* EntityTransform = GetTransform();
        const Transform* CameraTransform = Camera->GetTransform();
        const Vector3& CameraPosition = CameraTransform->GetPosition();
        const Vector3& EntityPosition = EntityTransform->GetPosition();
        const Vector3& EntityRotation = EntityTransform->GetRotation();
        const Vector3 CameraViewDirection = Math::Normalize(CameraPosition - EntityPosition);

        u32 PassIndex = 0;
        
        for (const LightComponent* Light : AllLights)
        {
            if (!Light->IsEnabled()) continue;
            
            if (PassIndex > 0)
            {
                Renderer->SetBlending(true);
                Renderer->SetBlendFunction(BlendMode::One, BlendMode::One, BlendOperation::Add);
            } else
            {
                Renderer->SetBlending(false);
            }

            for (const Mesh* Mesh : m_Model->GetMeshes())
            {
                Mesh->VertexArray->Bind();
                Mesh->VertexBuffer->Bind();
            
                m_Shader->Bind();
                if (const DirectionalLight* DirLight = dynamic_cast<const DirectionalLight*>(Light))
                    m_Shader->SetDirectionalLight("uDirectionalLight", DirLight);
         
                if (const PointLight* PtLight = dynamic_cast<const PointLight*>(Light))
                    m_Shader->SetPointLight("uPointLight", PtLight);
        
        
                if (const AmbientLight* AmbLight = dynamic_cast<const AmbientLight*>(Light))
                    m_Shader->SetAmbientLight("uAmbientLight", AmbLight);
                
                m_Shader->SetUniformMat4("uModel", GetTransform()->GetWorldSpaceMatrix());
                m_Shader->SetUniformFloat3("uCameraViewDirection", CameraViewDirection);
                const Matrix4 LocalToWorldNormal = Math::LocalToWorldNormal(EntityPosition, EntityRotation);
                m_Shader->SetUniformMat4("uLocalToWorldNormal", LocalToWorldNormal);
                
                Renderer->Draw(DrawMode::Triangles, Mesh->VertexArray, (i32)Mesh->Vertices.Count(), m_Shader);
            }
            
            PassIndex++;
        }
        
        
    }

    void ModelRenderer::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);
        if (UI::Button("Load Model"))
        {
            OpenFile();
        }
    }

    void ModelRenderer::OnModelChange(Model* NewModel)
    {
        
    }

    bool ModelRenderer::OpenFile()
    {
        const Path Filepath = File::OpenFileDialog("Open 3D mode file...", "", DialogFilters::ModelFilters);
        PopupMessage* Message = PopupMessage::Create("Warning", "Failed to open 3D model", PopupMessageResponse::OK, PopupMessageIcon::Warning);
            
        if (!File::Exists(Filepath))
        {
            if (Message) Message->Show();
            delete Message;
            return false;
        }
            
        Model* model = new Model("TransientModel");
        if (!model->LoadFromFile(Filepath))
        {
            if (Message) Message->Show();
            delete model;
            delete Message;
            return false;
        }

        if (m_Model)
        {
            delete m_Model;
            m_Model = model;
            OnModelChange(m_Model);
        }

        delete Message;
        return true;
    }

    Model* ModelRenderer::GetModel() const
    {
        return m_Model;
    }
}
