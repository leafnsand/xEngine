#include "FairyGUI/Component/FairyMeshComponent.h"
#include "FairyGUI/fairy/display/NGraphics.h"
#include "FairyGUI/fairy/ui/GObject.h"
#include "FairyGUI/fairy/render/RenderDataProcess.h"
#include "FairyGUI/fairy/display/DisplayObject.h"
#include "FairyGUI/EBus/FairySystemComponentBus.h"


#include <AzCore/Serialization/SerializeContext.h>
#include "Renderer/EBus/RendererComponentBus.h"
#include "Renderer/Base/Material.h"


namespace Module
{
    void FairyMeshComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serialize_context = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize_context->Class<FairyMeshComponent>()
                ->SerializerForEmptyClass();
        }
    }

    void FairyMeshComponent::Activate()
    {
        MeshProviderRequestBus::Handler::BusConnect(GetEntityId());
        FairyMeshRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());
        RendererSystemNotificationBus::Handler::BusConnect(GetEntityId());

        bgfx::VertexDecl vertexDecl;
        vertexDecl
            .begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
            .end();

        m_vertexBufferHandle = bgfx::createDynamicVertexBuffer(std::numeric_limits<AZ::u16>::max(), vertexDecl);
        m_indexBufferHandle = bgfx::createDynamicIndexBuffer(std::numeric_limits<AZ::u16>::max());
    }

    void FairyMeshComponent::Deactivate()
    {
        bgfx::destroy(m_vertexBufferHandle);
        bgfx::destroy(m_indexBufferHandle);

        MeshProviderRequestBus::Handler::BusDisconnect();
        FairyMeshRequestBus::Handler::BusDisconnect();
        AZ::TransformNotificationBus::Handler::BusDisconnect();
        RendererSystemNotificationBus::Handler::BusDisconnect();
    }

    void FairyMeshComponent::OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world)
    {
        world.StoreToColumnMajorFloat16Ex(m_modelTM);
    }

    void FairyMeshComponent::RefreshData(fairygui::NGraphics* graphics)
    {
        EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, ClearAll);
        // CaculateRenderData(graphics->GetTarget()->m_owner);
        m_meshContext.clear();
        int index = 0;
        AZStd::shared_ptr<Module::Material> material = nullptr;
        if (graphics->GetTarget()->m_context.type == fairygui::FairyRenderType::Fairy)
        {
            fairygui::RenderDataProcess::CaculateRenderData(graphics->GetTarget()->m_owner, fairygui::RenderDataProcess::GetSpriteFrame(graphics->GetTarget()->m_owner), this->m_meshContext, index);
            material = GetImageMaterial((Module::TextureAsset*)graphics->GetTarget()->m_context.texture->data, graphics->GetTarget()->m_context.maskIndex);
        }
        else if (graphics->GetTarget()->m_context.type == fairygui::FairyRenderType::Font)
        {
            fairygui::RenderDataProcess::CaculateRenderData(graphics->GetTarget()->m_owner, fairygui::RenderDataProcess::GetSpriteFrame(graphics->GetTarget()->m_owner), this->m_meshContext, index);
            material = GetFontMaterial((Module::TextureAsset*)graphics->GetTarget()->m_context.texture->data, graphics->GetTarget()->m_context.maskIndex);
        }
        else if (graphics->GetTarget()->m_context.type == fairygui::FairyRenderType::Shape)
        {
            fairygui::RenderDataProcess::CaculateShapeRect(graphics->GetTarget()->m_owner, this->m_meshContext, index);
            material = GetShapeMaterial( graphics->GetTarget()->m_context.maskIndex);
        }
        else if(graphics->GetTarget()->m_context.type == fairygui::FairyRenderType::Mask)
        {
            fairygui::RenderDataProcess::CaculateMaskRect(graphics->GetTarget()->m_owner, this->m_meshContext, index);
            material = GetMaskMaterial( graphics->GetTarget()->m_context.maskIndex);
        }
        if (material)
        {
            this->m_orderInLayer = graphics->GetTarget()->m_context.orderInLayer;
            EBUS_EVENT_ID(GetEntityId(), RendererRequestBus, SetOrderInLayer, graphics->GetTarget()->m_context.orderInLayer);
            EBUS_EVENT_ID(GetEntityId(), RendererRequestBus, SetMaterialAndMesh, material, 0);
        }
    }

    void FairyMeshComponent::CaculateRenderData(fairygui::GObject* g_object)
    {
     
      
    }


    void FairyMeshComponent::UpdateMeshBuffer()
    {
        if (m_meshContext.indexCount > 0)
        {
            bgfx::updateDynamicVertexBuffer(m_vertexBufferHandle, 0, bgfx::copy(m_meshContext.vt, m_meshContext.vertexCount * sizeof(fairygui::MeshContext::Vertex)));
            bgfx::updateDynamicIndexBuffer(m_indexBufferHandle, 0, bgfx::copy(m_meshContext.indices, m_meshContext.indexCount * sizeof(AZ::u16)));
        }
    }

    bool FairyMeshComponent::ApplySubMesh(int index)
    {
        float m_panelScale = 0;
        EBUS_EVENT_RESULT(m_panelScale, Module::FairySystemRequestBus, GetPanelScaleValue);
        AZ::Transform world = AZ::Transform::CreateIdentity();
        EBUS_EVENT_ID_RESULT(world, GetEntityId(), AZ::TransformBus, GetWorldTM);

        AZ::Transform tm = AZ::Transform::CreateScale(AZ::Vector3(0.01f * m_panelScale, 0.01f * m_panelScale, 1.0f));
        world = world * tm;

        float modelMatrix[16] = {};
        world.StoreToColumnMajorFloat16Ex(modelMatrix);

        bgfx::setTransform(modelMatrix);
        bgfx::setVertexBuffer(0, m_vertexBufferHandle);
        bgfx::setIndexBuffer(m_indexBufferHandle, 0, m_meshContext.indexCount);
        return true;
    }

    void FairyMeshComponent::OnPreRender()
    {
        if (m_meshContext.vertexCount>0)
        {
            UpdateMeshBuffer();
        }
    }

    AZStd::shared_ptr<Module::Material> FairyMeshComponent::GetImageMaterial(Module::TextureAsset* textureAsset, int stencilIndex)
    {
        if (textureAsset == nullptr)
        {
            return nullptr;
        }
        int index = textureAsset->GetHandle().idx;
        AZStd::string ext = AZStd::string::format("_%d_%d", stencilIndex, index);
        AZStd::shared_ptr<Material> material = nullptr;
        EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "fairy_normal" + ext);
        if (material == nullptr)
        {
            EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "default/materials/fairy_normal", "fairy_normal" + ext);
        }
        if (material)
        {
            material->SetTexture("s_texColor", textureAsset);
            if (stencilIndex > 0)
            {
                uint32_t fstencil =
                    BGFX_STENCIL_TEST_EQUAL
                    | BGFX_STENCIL_FUNC_REF(stencilIndex)
                    | BGFX_STENCIL_FUNC_RMASK(0xff)
                    | BGFX_STENCIL_OP_FAIL_S_KEEP
                    | BGFX_STENCIL_OP_FAIL_S_KEEP
                    | BGFX_STENCIL_OP_FAIL_S_KEEP;
                uint32_t bstencil = BGFX_STENCIL_NONE;
                material->SetStencil(fstencil, bstencil);
            }
        }
        return material;
    }

    AZStd::shared_ptr<Module::Material> FairyMeshComponent::GetFontMaterial(Module::TextureAsset* textureAsset, int stencilIndex)
    {
        if (textureAsset == nullptr)
        {
            return nullptr;
        }
        int index = textureAsset->GetHandle().idx;
        AZStd::string ext = AZStd::string::format("_%d_%d", stencilIndex, index);
        AZStd::shared_ptr<Material> material = nullptr;
        EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "font_normal" + ext);
        if (material == nullptr)
        {
            EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "default/materials/font_normal", "font_normal" + ext);
        }
        if (material)
        {
            material->SetTexture("s_texColor", textureAsset);
            if (stencilIndex > 0)
            {
                uint32_t fstencil =
                    BGFX_STENCIL_TEST_EQUAL
                    | BGFX_STENCIL_FUNC_REF(stencilIndex)
                    | BGFX_STENCIL_FUNC_RMASK(0xff)
                    | BGFX_STENCIL_OP_FAIL_S_KEEP
                    | BGFX_STENCIL_OP_FAIL_S_KEEP
                    | BGFX_STENCIL_OP_FAIL_S_KEEP;
                uint32_t bstencil = BGFX_STENCIL_NONE;
                material->SetStencil(fstencil, bstencil);
            }
        }
        return material;
    }

    AZStd::shared_ptr<Module::Material> FairyMeshComponent::GetShapeMaterial(int stencilIndex)
    {
        AZStd::string ext = AZStd::string::format("_1_%d", stencilIndex);
        AZStd::shared_ptr<Material> material = nullptr;
        EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "fairy_mask" + ext);
        if (material == nullptr)
        {
            EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "default/materials/fairy_mask", "fairy_mask" + ext);
        }
        if (material && (stencilIndex > 0))
        {
            uint32_t fstencil =
                BGFX_STENCIL_TEST_EQUAL
                | BGFX_STENCIL_FUNC_REF(stencilIndex)
                | BGFX_STENCIL_FUNC_RMASK(0xff)
                | BGFX_STENCIL_OP_FAIL_S_KEEP
                | BGFX_STENCIL_OP_FAIL_S_KEEP
                | BGFX_STENCIL_OP_FAIL_S_KEEP;
            uint32_t bstencil = BGFX_STENCIL_NONE;
            material->SetStencil(fstencil, bstencil);
        }
        return material;
    }

    AZStd::shared_ptr<Module::Material> FairyMeshComponent::GetMaskMaterial(int stencilIndex)
    {
        AZStd::shared_ptr<Material> material = nullptr;
        AZStd::string ext = AZStd::string::format("_%d", stencilIndex);
        EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "fairy_mask" + ext);
        if (material == nullptr)
        {
            EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "default/materials/fairy_mask", "fairy_mask" + ext);
        }
        if (material)
        {
            uint32_t fstencil =
                BGFX_STENCIL_TEST_ALWAYS
                | BGFX_STENCIL_FUNC_REF(stencilIndex)
                | BGFX_STENCIL_FUNC_RMASK(0xff)
                | BGFX_STENCIL_OP_FAIL_S_REPLACE
                | BGFX_STENCIL_OP_FAIL_Z_REPLACE
                | BGFX_STENCIL_OP_PASS_Z_REPLACE;
            uint32_t bstencil = BGFX_STENCIL_NONE;
            material->SetStencil(fstencil, bstencil);
        }
        return material;
    }

}