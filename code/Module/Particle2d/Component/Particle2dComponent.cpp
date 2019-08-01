#include "Particle2d/Component/Particle2dComponent.h"

#include "AzCore/Asset/AssetManager.h"
#include "Particle2d/Asset/Particle2dDescAssetHandler.h"
#include "Renderer/EBus/RendererComponentBus.h"
#include "AzCore/Serialization/SerializeContext.h"

namespace Module
{
    void Particle2dComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {

            serializeContext->Class<Particle2dComponent>()
                ->SerializerForEmptyClass();
        }
    }

    Particle2dComponent::Particle2dComponent(const AZStd::string& descPath)
    {
        m_descPath = descPath;
    }

    // AZ::Component
    void Particle2dComponent::Activate()
    {
        bgfx::VertexDecl vertexDecl;
        vertexDecl
            .begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
            .end();

        m_vertexBufferHandle = bgfx::createDynamicVertexBuffer(std::numeric_limits<AZ::u16>::max(), vertexDecl);
        m_indexBufferHandle = bgfx::createDynamicIndexBuffer(std::numeric_limits<AZ::u16>::max());

        Particle2dRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
        MeshProviderRequestBus::Handler::BusConnect(GetEntityId());
        Module::RendererSystemNotificationBus::Handler::BusConnect(GetEntityId());
        m_descAsset.Create((m_descPath + ".json").c_str());
        if (m_descAsset.GetId().IsValid())
        {
            AZ::Data::AssetBus::MultiHandler::BusConnect(m_descAsset.GetId());
            m_descAsset.QueueLoad();
        }
    }

    void Particle2dComponent::Deactivate()
    {
        Discard();
        if (!m_isValue)
        {
            AZ::Data::AssetBus::MultiHandler::BusDisconnect(m_descAsset.GetId());
        }
        m_descAsset.Release();
        Particle2dRequestBus::Handler::BusDisconnect();
        AZ::TransformNotificationBus::Handler::BusDisconnect();
        AZ::TickBus::Handler::BusDisconnect();
        MeshProviderRequestBus::Handler::BusDisconnect();
        Module::RendererSystemNotificationBus::Handler::BusDisconnect();
   

        bgfx::destroy(m_vertexBufferHandle);
        bgfx::destroy(m_indexBufferHandle);
    }


    // AZ::TransformNotificationBus::Handler
    void Particle2dComponent::OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world)
    {
        world.StoreToColumnMajorFloat16Ex(m_modelTM);
    }

    // AZ::TickBus::Handler
    void Particle2dComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
    {
        if (m_isRunning)
        {
            this->m_liveTime += deltaTime;
        }
        if (this->m_isValue)
        {
            CleanBuffer();
            for (int i = 0; i < m_nodeVector.size(); i++)
            {
                m_nodeVector[i]->Clean();
            }
            this->m_rootItem.OnUpdate(deltaTime);
            for (int i = 0; i < m_nodeVector.size(); i++)
            {
                m_nodeVector[i]->BRUpdate();
                int b = m_nodeVector[i]->VertexBufferDrawCount;
                for (int i = 0; i < b; i++)
                {
                    Particle2d::VertexData vd;
                    m_vertexBuffer.push_back(vd);
                }

                if (b > 0)
                {
                    AZStd::shared_ptr<Module::Material> material = m_nodeVector[i]->GetMaterial(m_nodeVector[i]->m_dataPtr->ShaderType, m_nodeVector[i]->m_textureAsset.GetAs<TextureAsset>());
                    if (material)
                    {
                        EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, SetMaterialAndMesh, material, i);
                    }
                }
            }
            this->m_rootItem.BRUpdate(deltaTime);
        
        }
    }
    // MeshProviderNotificationBus::Handler
    void Particle2dComponent::UpdateMeshBuffer()
    {
        if (m_vertexCount > 0 && m_indexCount > 0)
        {
            bgfx::updateDynamicVertexBuffer(m_vertexBufferHandle, 0, bgfx::copy(&m_vertexBuffer[0], m_vertexCount * sizeof(Particle2d::VertexData)));
            bgfx::updateDynamicIndexBuffer(m_indexBufferHandle, 0, bgfx::copy(&m_indexBuffer[0], m_indexCount * sizeof(AZ::u16)));
        }
    }

    bool Particle2dComponent::ApplySubMesh(int index)
    {
        bgfx::setVertexBuffer(0, m_vertexBufferHandle);
        bgfx::setIndexBuffer(m_indexBufferHandle, m_nodeVector[index]->VertexBufferStartIndex / 4 * 6, m_nodeVector[index]->TotalNeedRenderUnitNum * 6);
        return true;
    }

    // AZ::Data::AssetBus::MultiHandler
    void Particle2dComponent::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        if ((!m_isValue) && m_descAsset.IsReady())
        {
            Particle2d::Particle2DSystemDesc& desc = m_descAsset.GetAs<Particle2dDescAsset>()->GetDesc();
            for (int i = 0; i < desc.LauncherData.size(); i++)
            {
                Particle2d::Particle2dNode* node = new Particle2d::Particle2dNode();
                m_nodeVector.push_back(node);
            }
            for (int i = 0; i < desc.LauncherStruct.size(); i++)
            {
                if (desc.LauncherStruct[i]->Index == -1)
                {
                    for (int j = 0; j <desc.LauncherStruct[i]->ChildIndexArray.size(); j++)
                    {
                        this->m_childNodeVector.push_back(desc.LauncherStruct[i]->ChildIndexArray[j]);
                    }
                }
                else
                {
                    this->m_nodeVector[desc.LauncherStruct[i]->Index]->Init(this,desc.LauncherStruct[i], desc.LauncherData[desc.LauncherStruct[i]->Index]);
                }
            }
           
            m_rootItem.Setup(this,this->m_liveTime);
            m_isValue = true;
        }
        AZ::Data::AssetBus::MultiHandler::BusDisconnect(m_descAsset.GetId());
    }

    void Particle2dComponent::OnPreRender()
    {
        UpdateMeshBuffer();
    }

    // Particle2dRequestBus::Handler
    bool Particle2dComponent::IsValid() const
    {
        return m_isValue;
    }

    void Particle2dComponent::Stop()
    {
        if (this->m_isRunning)
        {
            this->m_isRunning = false;
        }
        Clean();
    }

    void Particle2dComponent::Restart()
    {
        if (m_hasClean==false)
        {
            Clean();
        }
        m_hasClean = false;
        this->m_isRunning = true;
    }

    void Particle2dComponent::Pause()
    {
        if (this->m_isRunning)
        {
            this->m_isRunning = false;
        }
    }
    
    void Particle2dComponent::Resume()
    {
        if (!this->m_isRunning)
        {
            this->m_isRunning = true;
        }
    }

    void Particle2dComponent::Clean()
    {
        for (int i = 0; i < m_nodeVector.size(); i++)
        {
            this->m_nodeVector[i]->Clean();
        }
        this->m_liveTime = 0;
        m_rootItem.Discard();
        m_rootItem.Setup(this, 0);
        m_hasClean = true;
    }

    void Particle2dComponent::CleanBuffer()
    {
        EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, ClearAll);
        m_vertexCount = 0;
        m_indexCount = 0;
        m_vertexBuffer.clear();
        m_indexBuffer.clear();
    }

    void Particle2dComponent::Discard()
    {
        this->m_isRunning = false;
        m_rootItem.Discard();
        for (int i = 0; i < m_nodeVector.size(); i++)
        {
            this->m_nodeVector[i]->Discard();
        }
    }
}
