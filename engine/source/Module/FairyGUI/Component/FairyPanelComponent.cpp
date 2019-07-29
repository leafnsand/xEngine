#include "FairyGUI/Component/FairyPanelComponent.h"

#include "Renderer/Asset/TextureAsset.h"
#include "Renderer/Base/Material.h"

#include "AzCore/Serialization/SerializeContext.h"
#include "AzCore/StringFunc/StringFunc.h"
#include "Renderer/EBus/RendererComponentBus.h"
#include "EBus/FairySystemComponentBus.h"
#include "FairyGUI/fairy/display/SpriteFrame.h"
#include "Window/EBus/WindowSystemComponentBus.h"
#include "display/FonsFont.h"
#include "utils/FileUtils.h"
#include "base/AutoreleasePool.h"

namespace Module
{
    void FairyPanelComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<FairyPanelComponent, AZ::Component>()
                ->Field("packageName", &FairyPanelComponent::m_packageName)
                ->Field("componentName", &FairyPanelComponent::m_componentName)
                ->Field("renderMode", &FairyPanelComponent::m_renderMode)
                ->Field("sortingOrder", &FairyPanelComponent::m_sortingOrder)
                ->Field("position", &FairyPanelComponent::m_position)
                ->Field("scale", &FairyPanelComponent::m_scale)
                ->Field("rotation", &FairyPanelComponent::m_rotation)
                ->Field("fitScreen", &FairyPanelComponent::m_fitScreen)
                ->Field("touchDisabled", &FairyPanelComponent::m_touchDisabled)
                ;
        }
    }

    // AZ::Component
    void FairyPanelComponent::Activate()
    {
        MeshProviderRequestBus::Handler::BusConnect(GetEntityId());
        FairyPanelRequestBus::Handler::BusConnect(GetEntityId());
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

        EBUS_EVENT_RESULT(m_panelScale, Module::FairySystemRequestBus, GetPanelScaleValue);
        if (m_container == nullptr)
        {
            m_container = new fairygui::Container(this->GetEntityId(), nullptr);
        }
        
    }

    void FairyPanelComponent::Deactivate()
    {
        if (m_container != nullptr)
        {
            delete m_container;
            m_container = nullptr;
        }
        MeshProviderRequestBus::Handler::BusDisconnect();
        FairyPanelRequestBus::Handler::BusDisconnect();
        AZ::TransformNotificationBus::Handler::BusDisconnect();
        RendererSystemNotificationBus::Handler::BusDisconnect();
    }

    void FairyPanelComponent::SetMaterialAndMesh(AZStd::shared_ptr<Module::Material> material, int subMeshIndex)
    {
        EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, SetMaterialAndMesh, material, subMeshIndex);
    }

    void FairyPanelComponent::ClearMaterialAndMesh()
    {
        EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, ClearAll);
    }

    // AZ::TransformNotificationBus::Handler
    void FairyPanelComponent::OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world)
    {
        world.StoreToColumnMajorFloat16Ex(m_modelTM);
    }

    // MeshProviderNotificationBus::Handler
    void FairyPanelComponent::UpdateMeshBuffer()
    {
        m_showMask = false;
        m_maskRect = Rect::ZERO;
        if (m_meshContext.indexCount > 0)
        {
            bgfx::updateDynamicVertexBuffer(m_vertexBufferHandle, 0, bgfx::copy(m_meshContext.vt, m_meshContext.vertexCount * sizeof(MeshContext::Vertex)));
            bgfx::updateDynamicIndexBuffer(m_indexBufferHandle, 0, bgfx::copy(m_meshContext.indices, m_meshContext.indexCount * sizeof(AZ::u16)));
        }
    }

    bool FairyPanelComponent::ApplySubMesh(int index)
    {
        if (m_vecRenderObj.size() <= index)
        {
            return false;
        }
        int maskIndex = 0;

        AZ::Transform world = AZ::Transform::CreateIdentity();
        EBUS_EVENT_ID_RESULT(world, GetEntityId(), AZ::TransformBus, GetWorldTM);
        AZ::Transform tm = AZ::Transform::CreateScale(AZ::Vector3(0.01f * m_panelScale, 0.01f * m_panelScale, 1.0f));
        world = world * tm;

        float modelMatrix[16] = {};
        world.StoreToColumnMajorFloat16Ex(modelMatrix);

        bgfx::setTransform(modelMatrix);
        bgfx::setVertexBuffer(0, m_vertexBufferHandle);


        int a = m_vecRenderObj.size();
        auto& context = m_vecRenderObj.at(index);
        /*
        if (m_showMask)
        {
            bgfx::setScissor(m_maskRect.origin.x, m_maskRect.origin.y, m_maskRect.size.width, m_maskRect.size.height);
        }
        */
        switch (context.type)
        {
        case FairyRenderType::Fairy:
            bgfx::setIndexBuffer(m_indexBufferHandle, context.fairyContext.startIndex, context.fairyContext.numIndex);
            break;
        case FairyRenderType::Font:
            bgfx::setIndexBuffer(m_indexBufferHandle, context.fontContext.startIndex, context.fontContext.numIndex);
            break;
        case FairyRenderType::Mask:
            bgfx::setIndexBuffer(m_indexBufferHandle, context.maskContext.startIndex, context.maskContext.numIndex);
            break;
        case FairyRenderType::Shape:
            bgfx::setIndexBuffer(m_indexBufferHandle, context.shapeContext.startIndex, context.shapeContext.numIndex);
            break;
        default:
            break;
            // bgfx::setVertexBuffer(0, m_vertexBufferHandle);
        }

        return true;
    }

    // new

    void FairyPanelComponent::SetConfigData(AZStd::string packageName, AZStd::string componentName)
    {
        this->m_packageName = packageName;
        this->m_componentName = componentName;
    }

    void FairyPanelComponent::SetRenderMode(fairygui::FairyRenderMode renderMode)
    {
        this->m_renderMode = renderMode;
    }

    void FairyPanelComponent::SetSortingOrder(int value, bool apply)
    {
        this->m_sortingOrder = value;
    }

    void FairyPanelComponent::CreateUI()
    {
        if (m_ui != nullptr)
        {
            delete m_ui;
            m_ui = nullptr;
        }
        CreateUI_PlayMode();
    }

    void FairyPanelComponent::CreateUI_PlayMode()
    {
        if (m_packageName.empty() || m_componentName.empty())
        {
            Ref* targetUI;
            EBUS_EVENT_RESULT(targetUI, FairyPackageSystemRequestBus, CreateEmptyObject, "component");
            if (targetUI != nullptr)
            {
                m_ui = (GComponent*)targetUI;
                GRoot* fairyGUIRoot = nullptr;
                EBUS_EVENT_RESULT(fairyGUIRoot, FairyPackageSystemRequestBus, GetGRoot);
                fairyGUIRoot->addChild(m_ui, this->m_container);
            }
        }
        else
        {
            GObject* targetUI;
            EBUS_EVENT_RESULT(targetUI, FairyPackageSystemRequestBus, CreateObject, m_packageName, m_componentName);
            if (targetUI != nullptr)
            {
                m_ui = (GComponent*)targetUI;
                GRoot* fairyGUIRoot = nullptr;
                EBUS_EVENT_RESULT(fairyGUIRoot, FairyPackageSystemRequestBus, GetGRoot);
                fairyGUIRoot->addChild(m_ui, this->m_container);
            }
        }
    }

    void FairyPanelComponent::OnPreRender()
    {
        // EBUS_EVENT_ID(GetEntityId(), AZ::TransformBus, SetLocalScale, AZ::Vector3(0.01f, 0.01f, 0.01f));
        EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, ClearAll);
        this->ReBuild();
        if (!m_vecRenderObj.empty())
        {
            UpdateMeshBuffer();
        }
    }

    void FairyPanelComponent::ReBuild()
    {
        m_renderNode = nullptr;
        m_currentMaskNode = nullptr;
        m_maskIndex = 0;
        m_meshContext.clear();
        m_vecRenderObj.clear();
        m_currentRenderType = FairyRenderType::Fairy;

        if (m_ui)
        {
            this->CollectData(m_ui->getChildren());
            BatchNode::RefresDisplayObjs(m_renderNode);
            /*
            BatchNode::OutputVertexAndIndices(m_renderNode, m_meshContext, m_vecRenderObj);
            // AZ::Debug::Trace::Instance().Printf("RenderNode", "->  %d\n", m_vecRenderObj.size());
            for (int i = 0; i < m_vecRenderObj.size(); i++)
            {
                if (m_vecRenderObj[i].type == FairyRenderType::Fairy)
                {
                    AZStd::shared_ptr<Module::Material> material = GetMaterial(MaterialType::Fairy_Normal, (Module::TextureAsset*)m_vecRenderObj[i].fairyContext.texture->data, m_vecRenderObj[i].fairyContext.maskIndex);
                    if (material)
                    {
                        EBUS_EVENT_ID(GetEntityId(), RendererRequestBus, SetMaterialAndMesh, material, i);
                    }
                }
                else if (m_vecRenderObj[i].type == FairyRenderType::Font)
                {
                    AZStd::shared_ptr<Module::Material> material = GetMaterial(MaterialType::Font_Normal, (Module::TextureAsset*)m_vecRenderObj[i].fontContext.texture->data, m_vecRenderObj[i].fontContext.maskIndex);
                    if (material)
                    {
                        EBUS_EVENT_ID(GetEntityId(), RendererRequestBus, SetMaterialAndMesh, material, i);
                    }
                }
                else if (m_vecRenderObj[i].type == FairyRenderType::Mask)
                {
                    if (m_vecRenderObj[i].maskContext.applyMask > 0)
                    {
                        AZStd::shared_ptr<Module::Material> material = GetMaskMaterial(m_vecRenderObj[i].maskContext.applyMask);
                        if (material)
                        {
                            EBUS_EVENT_ID(GetEntityId(), RendererRequestBus, SetMaterialAndMesh, material, i);
                        }
                    }
                }
                else if (m_vecRenderObj[i].type == FairyRenderType::Shape)
                {
                    AZStd::shared_ptr<Module::Material> material = GetShapeMaterial(m_vecRenderObj[i].shapeContext.maskIndex);
                    if (material)
                    {
                        EBUS_EVENT_ID(GetEntityId(), RendererRequestBus, SetMaterialAndMesh, material, i);
                    }
                }
            }
            */
            m_maskIndex = 0;
            m_renderNode = nullptr;
            m_currentMaskNode = nullptr;
        }
    }

    void FairyPanelComponent::DealWithChildren(GComponent* parent)
    {
        auto orderType = parent->getChildrenRenderOrder();
        if (orderType == ChildrenRenderOrder::ASCENT)
        {
            CollectData(parent->getChildren());
        }
        else if (orderType == ChildrenRenderOrder::DESCENT)
        {
            auto childrens = parent->getChildren();
            Vector<GObject*> tmp;
            for (auto child : childrens)
            {
                tmp.insert(0, child);
            }
            CollectData(tmp);
        }
        else if (orderType == ChildrenRenderOrder::ARCH)
        {
            auto childrens = parent->getChildren();
            int apexIndex = parent->getApexIndex();
            int nowIndex = 0;
            int allSize = childrens.size();
            Vector<GObject*> tmp;
            for (auto child : childrens)
            {
                if (nowIndex < apexIndex)
                {
                    tmp.pushBack(child);
                }
                else
                {
                    tmp.insert(apexIndex + allSize - 1 - nowIndex, child);
                }
                ++nowIndex;
            }
            CollectData(tmp);
        }
    }

    bool FairyPanelComponent::CollectData(const Vector<GObject*>& vec)
    {
        for (auto go : vec)
        {
            if (!go || !go->isVisible()) continue;

            FairyRenderType lastType = m_currentRenderType;
            if (this->IsFairyExtension(go, m_currentRenderType))
            {
                BatchNode* node = new BatchNode(m_currentRenderType, go);
                m_renderNode = BatchNode::CheckInsertNode(m_renderNode, node);
            }
            else if(go->SafeCast<GGraph>())
            {
                ShapeNode sn;
                sn.go = go;
                sn.isGrayed = go->isGrayed();
                sn.obbRect = RenderDataProcess::CaculateRenderRect(go);
                sn.m_maskIndex = m_curMaskIndex;
                BatchNode* node = new BatchNode(FairyRenderType::Shape, sn);
                m_renderNode = BatchNode::CheckInsertNode(m_renderNode, node);
            }
            else if (RenderDataProcess::IsNeedRender(go))
            {
                SpriteFrame* pSprite = RenderDataProcess::GetSpriteFrame(go);
                if (pSprite && pSprite->texture)
                {
                    bool isFont = FonsFont::getInstance()->IsFontSpriteFrame(pSprite);
                    if (isFont)
                    {
                        bool isAutoSize = RenderDataProcess::IsTextAutoSize(go);
                        if (!isAutoSize)
                            this->BeginMask(go);

                        FontNode fn;
                        fn.go = go;
                        RenderDataProcess::GetFontEffect(fn);
                        fn.obbRect = RenderDataProcess::CaculateRenderRect(go);
                        fn.texture = pSprite->texture;
                        fn.m_maskIndex = m_curMaskIndex;
                        BatchNode* node = new BatchNode(FairyRenderType::Font, fn);
                        m_renderNode = BatchNode::CheckInsertNode(m_renderNode, node);

                        if (GTextInput* pinput = go->SafeCast<GTextInput>())
                        {
                            if (pinput->IsEditing())
                            {
                                Vector<GObject*> tmpVec;
                                tmpVec.pushBack(pinput->GetCursor());
                                CollectData(tmpVec);
                            }
                        }

                        if (!isAutoSize)
                            this->EndMask();
                    }
                    else
                    {
                        bool isTile = RenderDataProcess::IsTileRender(go);
                        if (isTile)
                            this->BeginMask(go);

                        FairyNode fn;
                        fn.go = go;
                        fn.isGrayed = go->isGrayed();
                        fn.obbRect = RenderDataProcess::CaculateRenderRect(go);
                        fn.texture = pSprite->texture;
                        fn.m_maskIndex = m_curMaskIndex;
                        BatchNode* node = new BatchNode(FairyRenderType::Fairy, fn);
                        m_renderNode = BatchNode::CheckInsertNode(m_renderNode, node);
                     
                        if (isTile)
                            this->EndMask();
                    }
                }
            }

            GComponent* pc = go->SafeCast<GComponent>();
            if (pc)
            {
                bool isClipping = pc->isClippingEnabled();
                if (isClipping)
                {
                    this->BeginMask(pc);
                }
                GObject* pMask = pc->getMask();
                if (pMask)
                {
                    this->BeginMask(pMask);
                    DealWithChildren(pc);
                    this->EndMask();
                }
                else
                {
                    DealWithChildren(pc);
                }
                if (isClipping)
                {
                    this->EndMask();
                }
            }
        }
        return false;
    }

    bool FairyPanelComponent::IsFairyExtension(GObject* obj, FairyRenderType& type)
    {
        if (obj->SafeCast<GSpine>())
        {
            type = FairyRenderType::Spine;
            return true;
        }
        else if (obj->SafeCast<GParticle2d>())
        {
            type = FairyRenderType::Particle2d;
            return true;
        }
        else if (obj->SafeCast<GGraph>())
        {
            type = FairyRenderType::Shape;
            return false;
        }

        type = FairyRenderType::Fairy;
        return false;
    }

    void FairyPanelComponent::BeginMask(GObject* pMask)
    {
        MaskNode mask;
        mask.parent = m_currentMaskNode;
        mask.index = ++m_maskIndex;
        mask.go = pMask;
        // RenderDataProcess::CaculateMaskRect(pMask, &mask);
        BatchNode* node = new BatchNode(FairyRenderType::Mask, mask);
        m_renderNode = BatchNode::CheckInsertNode(m_renderNode, node);
        m_currentMaskNode = &(node->maskNode);
        m_curMaskIndex = m_maskIndex;
    }

    void FairyPanelComponent::EndMask()
    {
        MaskNode* pNode = m_currentMaskNode->parent;
        MaskNode mask;
        mask.index = 0; // pNode != nullptr ? ++m_maskIndex : 0;
        mask.rect = pNode != nullptr ? pNode->rect : Rect::ZERO;
        mask.parent = nullptr;
        BatchNode* node = new BatchNode(FairyRenderType::Mask, mask);
        m_renderNode = BatchNode::CheckInsertNode(m_renderNode, node);

        m_currentMaskNode = pNode;
        m_curMaskIndex = 0;
    }

    using Module::Material;

    AZStd::shared_ptr<Module::Material> FairyPanelComponent::GetMaskMaterial(int maskIndex)
    {
        AZStd::shared_ptr<Material> material = nullptr;
        AZStd::string ext = AZStd::string::format("_%d", maskIndex);
        EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "fairy_mask" + ext);
        if (material == nullptr)
        {
            EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "default/materials/fairy_mask", "fairy_mask" + ext);
        }
        if (material)
        {
            uint32_t fstencil = 
                  BGFX_STENCIL_TEST_ALWAYS         
                | BGFX_STENCIL_FUNC_REF(maskIndex)
                | BGFX_STENCIL_FUNC_RMASK(0xff)
                | BGFX_STENCIL_OP_FAIL_S_REPLACE
                | BGFX_STENCIL_OP_FAIL_Z_REPLACE
                | BGFX_STENCIL_OP_PASS_Z_REPLACE;  
            uint32_t bstencil = BGFX_STENCIL_NONE;
            material->SetStencil(fstencil, bstencil);
        }
        return material;
    }

    AZStd::shared_ptr<Module::Material> FairyPanelComponent::GetShapeMaterial(int maskIndex)
    {
        AZStd::string ext = AZStd::string::format("_1_%d", maskIndex);
        AZStd::shared_ptr<Material> material = nullptr;
        EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "fairy_mask" + ext);
        if (material == nullptr)
        {
            EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "default/materials/fairy_mask", "fairy_mask" + ext);
        }
        if (material && (maskIndex > 0))
        {
            uint32_t fstencil =
                BGFX_STENCIL_TEST_EQUAL
                | BGFX_STENCIL_FUNC_REF(maskIndex)
                | BGFX_STENCIL_FUNC_RMASK(0xff)
                | BGFX_STENCIL_OP_FAIL_S_KEEP
                | BGFX_STENCIL_OP_FAIL_S_KEEP
                | BGFX_STENCIL_OP_FAIL_S_KEEP;
            uint32_t bstencil = BGFX_STENCIL_NONE;
            material->SetStencil(fstencil, bstencil);
        }
        return material;
    }

    AZStd::shared_ptr<Module::Material> FairyPanelComponent::GetMaterial(MaterialType materialType, Module::TextureAsset* textureAsset,int maskIndex)
    {
        if (textureAsset == nullptr)
        {
            return nullptr;
        }
        int index = textureAsset->GetHandle().idx;
        AZStd::string ext = AZStd::string::format("_%d_%d", maskIndex, index);
        AZStd::shared_ptr<Material> material = nullptr;
        switch (materialType)
        {
        case MaterialType::Fairy_Normal:
            EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "fairy_normal" + ext);
            if (material == nullptr)
            {
                EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "default/materials/fairy_normal", "fairy_normal" + ext);
            }

            break;
        case MaterialType::Font_Normal:
            EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "font_normal" + ext);
            if (material == nullptr)
            {
                EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "default/materials/font_normal", "font_normal" + ext);
            }
            break;
        }
        if (material)
        {
            material->SetTexture("s_texColor", textureAsset);
            if (maskIndex > 0)
            {
                uint32_t fstencil =
                    BGFX_STENCIL_TEST_EQUAL
                    | BGFX_STENCIL_FUNC_REF(maskIndex)
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
}
