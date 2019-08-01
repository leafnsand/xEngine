#include "Particle2d/Priv/Particle2dNode.h"
#include "Component/Particle2dComponent.h"

namespace Particle2d
{
    Particle2dNode::Particle2dNode()
    {
  
    }

    Particle2dNode::~Particle2dNode()
    {
        
    }

    void Particle2dNode::Init(Module::Particle2dComponent* componentPtr, Particle2DStructData* structPtr, Particle2DLauncherData* dataPtr)
    {
        m_component = componentPtr;
        this->m_dataPtr = dataPtr;
        int childNum = structPtr->ChildIndexArray.size();
        if (childNum > 0)
        {
            for (int i = 0; i < childNum; i++)
            {
                m_childNodeVector.push_back(structPtr->ChildIndexArray[i]);
            }
        }
        
        AZStd::string uri = AZStd::string::format("%s%s.dds", "particle2d/", this->m_dataPtr->ImageName.c_str());
        m_textureAsset.Create(uri.c_str());
        if (m_textureAsset.GetId().IsValid())
        {
            AZ::Data::AssetBus::Handler::BusConnect(m_textureAsset.GetId());
            m_textureAsset.QueueLoad();
        }
        dataPtr->ShaderType;
    }

    using Module::Material;

    AZStd::shared_ptr<Module::Material> Particle2dNode::GetMaterial(AZStd::string& name, Module::TextureAsset* textureAsset)
    {
        if (textureAsset == nullptr)
        {
            return nullptr;
        }
        int index = textureAsset->GetHandle().idx;
        AZStd::string ext = AZStd::string::format("_%d", index);
        AZStd::shared_ptr<Material> material = nullptr;
        EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, name + ext);
        if (material == nullptr)
        {
            EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, name, name + ext);
        }
        if (material)
        {
            material->SetTexture("s_texColor", textureAsset);
        }
        return material;
    }

    void Particle2dNode::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        AZ::Data::AssetBus::Handler::BusDisconnect(asset.GetId());
    }

    void Particle2dNode::Discard()
    {
        m_textureAsset.Release();
    }

    void Particle2dNode::BRUpdate()
    {
        this->VertexBufferStartIndex = m_component->m_vertexCount;//                          Particle2DMgr::ptr->VertexCount;
        this->CurVertexBufferIndex = m_component->m_vertexCount; //                           Particle2DMgr::ptr->VertexCount;
        m_component->m_vertexCount += this->TotalNeedRenderUnitNum * 4;
        this->VertexBufferDrawCount = this->TotalNeedRenderUnitNum * 4;
    }

    int Particle2dNode::GetVertexBufferIndex()
    {
        int returnIndex = this->CurVertexBufferIndex;
        this->CurVertexBufferIndex += 4;
        return returnIndex;
    }

    void Particle2dNode::Clean()
    {
        this->CurVertexBufferIndex = 0;
        this->VertexBufferDrawCount = 0;
        this->VertexBufferStartIndex = 0;
        this->TotalNeedRenderUnitNum = 0;
    }
}
