#include "Spine/Component/SpineComponent.h"
#include "Window/EBus/WindowSystemComponentBus.h"
#include "Renderer/EBus/RendererComponentBus.h"
#include "Renderer/Asset/TextureAsset.h"
#include "Renderer/Base/Material.h"

#include "spine/spine.h"
#include "spine/extension.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/StringFunc/StringFunc.h>

#include <bx/math.h>

namespace Module
{
    static const uint16_t QuadIndices[] = { 0, 1, 2, 0, 2, 3 };

    static void OnAnimationEvent(spAnimationState* state, spEventType type, spTrackEntry* entry, spEvent* event)
    {
        auto self = (SpineComponent*)state->userData;
        if (type == SP_ANIMATION_START)
        {
            EBUS_EVENT_ID(self->GetEntityId(), SpineNotificationBus, OnAnimationEvent, entry->animation->name, SpineAnimationEventType::Start);
        }
        else if (type == SP_ANIMATION_END)
        {
            EBUS_EVENT_ID(self->GetEntityId(), SpineNotificationBus, OnAnimationEvent, entry->animation->name, SpineAnimationEventType::End);
        }
        else if (type == SP_ANIMATION_COMPLETE)
        {
            EBUS_EVENT_ID(self->GetEntityId(), SpineNotificationBus, OnAnimationEvent, entry->animation->name, SpineAnimationEventType::Complete);
        }
        else if (type == SP_ANIMATION_EVENT)
        {
            AZ_Warning("SpineComponent", false, "spine anim event not implemented!");
        }
    }


    static AZStd::shared_ptr<Material> GetMaterial(int blendMode, TextureAsset* texture)
    {
        AZStd::shared_ptr<Material> material = nullptr;
        if(texture != nullptr)
        {
            int index = texture->GetHandle().idx;
            AZStd::string ext = AZStd::string::format("_%d", index); 
            switch (blendMode)
            {
            case SP_BLEND_MODE_NORMAL:
                EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "spine_normal" + ext);
                if (material == nullptr)
                {
                    EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "spine_normal", "spine_normal" + ext);
                    if (material)
                    {
                        Material::Property property;
                        property.m_type = Material::Property::Type::Texture;
                        texture->Acquire();
                        property.m_textureValue = texture;
                        material->SetProperty(property);
                    }
                }
                break;
            case SP_BLEND_MODE_ADDITIVE:
                EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "spine_additive" + ext);
                if (material == nullptr)
                {
                    EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "spine_additive", "spine_additive" + ext);
                    if (material)
                    {
                        Material::Property property;
                        property.m_type = Material::Property::Type::Texture;
                        texture->Acquire();
                        property.m_textureValue = texture;
                        material->SetProperty(property);
                    }
                }
                break;
            case SP_BLEND_MODE_MULTIPLY:
                EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "spine_multiply" + ext);
                if (material == nullptr)
                {
                    EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "spine_multiply", "spine_multiply" + ext);
                    if (material)
                    {
                        Material::Property property;
                        property.m_type = Material::Property::Type::Texture;
                        texture->Acquire();
                        property.m_textureValue = texture;
                        material->SetProperty(property);
                    }
                }
                break;
            case SP_BLEND_MODE_SCREEN:
                EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "spine_screen" + ext);
                if (material == nullptr)
                {
                    EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "spine_screen", "spine_screen" + ext);
                    if (material)
                    {
                        Material::Property property;
                        property.m_type = Material::Property::Type::Texture;
                        texture->Acquire();
                        property.m_textureValue = texture;
                        material->SetProperty(property);
                    }
                }
                break;
            default:
                EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "spine_normal" + ext);
                if (material == nullptr)
                {
                    EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "spine_normal", "spine_normal" + ext);
                    if (material)
                    {
                        Material::Property property;
                        property.m_type = Material::Property::Type::Texture;
                        texture->Acquire();
                        property.m_textureValue = texture;
                        material->SetProperty(property);
                    }
                }
                break;
            }
        }

        return material;
    }

    void SpineComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<SpineComponent, AZ::Component>()
                ->Field("name", &SpineComponent::m_name)
                ;
        }
    }

    SpineComponent::SpineComponent()
    {
    }

    SpineComponent::SpineComponent(const AZStd::string& name)
    {
        m_name = name;
        m_atlasAsset.Create((m_name + ".atlas").c_str());
        m_skeletonAsset.Create((m_name + ".json").c_str());
    }

    void SpineComponent::Activate()
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

        if (m_atlasAsset.GetId().IsValid())
        {
            AZ::Data::AssetBus::MultiHandler::BusConnect(m_atlasAsset.GetId());
            m_atlasAsset.QueueLoad();
        }
        if (m_skeletonAsset.GetId().IsValid())
        {
            AZ::Data::AssetBus::MultiHandler::BusConnect(m_skeletonAsset.GetId());
            m_skeletonAsset.QueueLoad();
        }

        SpineRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
        MeshProviderRequestBus::Handler::BusConnect(GetEntityId());
    }

    void SpineComponent::Deactivate()
    {
        SpineRequestBus::Handler::BusDisconnect();
        AZ::TransformNotificationBus::Handler::BusDisconnect();
        AZ::TickBus::Handler::BusDisconnect();
        MeshProviderRequestBus::Handler::BusDisconnect();

        bgfx::destroy(m_vertexBufferHandle);
        bgfx::destroy(m_indexBufferHandle);

        if (m_state && m_state->data) spAnimationStateData_dispose(m_state->data);
        if (m_state) spAnimationState_dispose(m_state);
        if (m_skeleton) spSkeleton_dispose(m_skeleton);
    }

    void SpineComponent::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        if (!IsValid() && m_atlasAsset.IsReady() && m_skeletonAsset.IsReady())
        {
            m_atlas = spAtlas_create((const char*)m_atlasAsset.Get()->GetBuffer(), m_atlasAsset.Get()->GetLength(), "", nullptr);
            spSkeletonJson* json = spSkeletonJson_create(m_atlas);
            spSkeletonData* skeletonData = spSkeletonJson_readSkeletonData(json, (const char*)m_skeletonAsset.Get()->GetBuffer());
            spSkeletonJson_dispose(json);
            m_skeleton = spSkeleton_create(skeletonData);
            spAnimationStateData* pAnim = spAnimationStateData_create(skeletonData);
            m_state = spAnimationState_create(pAnim);

            m_state->listener = OnAnimationEvent;
            m_state->userData = this;

            SpineRequestBus::ExecuteQueuedEvents();
        }
    }

    bool SpineComponent::IsValid() const
    {
        return m_atlas && m_skeleton && m_state;
    }

    void SpineComponent::SetSkin(const AZStd::string& skinName)
    {
        if (!IsValid())
        {
            EBUS_QUEUE_EVENT_ID(GetEntityId(), SpineRequestBus, SetSkin, skinName);
            return;
        }
        spSkeleton_setSkinByName(m_skeleton, skinName.c_str());
    }

    void SpineComponent::SetMix(const AZStd::string& fromAnim, const AZStd::string& toAnim, float duration)
    {
        if (!IsValid())
        {
            EBUS_QUEUE_EVENT_ID(GetEntityId(), SpineRequestBus, SetMix, fromAnim, toAnim, duration);
            return;
        }
        spAnimationStateData_setMixByName(m_state->data, fromAnim.c_str(), toAnim.c_str(), duration);
    }

    void SpineComponent::ClearTracks()
    {
        if (!IsValid())
        {
            EBUS_QUEUE_EVENT_ID(GetEntityId(), SpineRequestBus, ClearTracks);
            return;
        }
        spAnimationState_clearTracks(m_state);
    }

    void SpineComponent::ClearTrack(int trackIndex)
    {
        if (!IsValid())
        {
            EBUS_QUEUE_EVENT_ID(GetEntityId(), SpineRequestBus, ClearTrack, trackIndex);
            return;
        }
        spAnimationState_clearTrack(m_state, trackIndex);
    }

    void SpineComponent::SetAnimation(int trackIndex, const AZStd::string& animationName, bool loop)
    {
        if (!IsValid())
        {
            EBUS_QUEUE_EVENT_ID(GetEntityId(), SpineRequestBus, SetAnimation, trackIndex, animationName, loop);
            return;
        }
        spAnimationState_setAnimationByName(m_state, trackIndex, animationName.c_str(), loop);
    }

    void SpineComponent::AddAnimation(int trackIndex, const AZStd::string& animationName, bool loop, float delay)
    {
        if (!IsValid())
        {
            EBUS_QUEUE_EVENT_ID(GetEntityId(), SpineRequestBus, AddAnimation, trackIndex, animationName, loop, delay);
            return;
        }
        spAnimationState_addAnimationByName(m_state, trackIndex, animationName.c_str(), loop, delay);
    }

    void SpineComponent::SetEmptyAnimation(int trackIndex, float mixDuration)
    {
        if (!IsValid())
        {
            EBUS_QUEUE_EVENT_ID(GetEntityId(), SpineRequestBus, SetEmptyAnimation, trackIndex, mixDuration);
            return;
        }
        spAnimationState_setEmptyAnimation(m_state, trackIndex, mixDuration);
    }

    void SpineComponent::AddEmptyAnimation(int trackIndex, float mixDuration, float delay)
    {
        if (!IsValid())
        {
            EBUS_QUEUE_EVENT_ID(GetEntityId(), SpineRequestBus, AddEmptyAnimation, trackIndex, mixDuration, delay);
            return;
        }
        spAnimationState_addEmptyAnimation(m_state, trackIndex, mixDuration, delay);
    }

    void SpineComponent::SetEmptyAnimations(float mixDuration)
    {
        if (!IsValid())
        {
            EBUS_QUEUE_EVENT_ID(GetEntityId(), SpineRequestBus, SetEmptyAnimations, mixDuration);
            return;
        }
        spAnimationState_setEmptyAnimations(m_state, mixDuration);
    }

    void SpineComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
    {
        if (!IsValid()) return;

        m_contextQueue.clear();
        m_vertexBuffer.clear();
        m_indexBuffer.clear();

        EBUS_EVENT_ID(GetEntityId(), RendererRequestBus, ClearAll);

        spSkeleton_update(m_skeleton, deltaTime);
        spAnimationState_update(m_state, deltaTime);
        spAnimationState_apply(m_state, m_skeleton);
        spSkeleton_updateWorldTransform(m_skeleton);

        auto flushBlendMode = SP_BLEND_MODE_NORMAL;
        void* flushTexture = nullptr;

        int drawStartIndex = m_indexBuffer.size();
        int drawIndexCount = 0;

        const int slotCount = m_skeleton->slotsCount;
        for (int i = 0; i < slotCount; ++i)
        {
            auto slot = m_skeleton->drawOrder[i];
            if (slot && slot->attachment)
            {
                const auto currentBlendMode = slot->data->blendMode;
                void* currentTexture = nullptr;

                int numVertices = 0, numIndices = 0;

                const auto type = slot->attachment->type;
                if (type == SP_ATTACHMENT_REGION)
                {
                    auto attachment = SUB_CAST(spRegionAttachment, slot->attachment);
                    currentTexture = SUB_CAST(spAtlasRegion, attachment->rendererObject)->page->rendererObject;

                    numVertices = 4;
                    numIndices = 6;

                    auto vertexSize = m_vertexBuffer.size();
                    auto indexSize = m_indexBuffer.size();

                    m_vertexBuffer.resize(vertexSize + numVertices);
                    m_indexBuffer.resize(indexSize + numIndices);

                    spRegionAttachment_computeWorldVertices(attachment, slot->bone, (float*)&m_vertexBuffer[vertexSize], 0, sizeof(Vertex) / sizeof(float));

                    for (auto index = 0; index < numVertices; ++index)
                    {
                        m_vertexBuffer[vertexSize + index].u = attachment->uvs[index * 2];
                        m_vertexBuffer[vertexSize + index].v = attachment->uvs[index * 2 + 1];
                        m_vertexBuffer[vertexSize + index].r = m_skeleton->color.r * slot->color.r * attachment->color.r;
                        m_vertexBuffer[vertexSize + index].g = m_skeleton->color.g * slot->color.g * attachment->color.g;
                        m_vertexBuffer[vertexSize + index].b = m_skeleton->color.b * slot->color.b * attachment->color.b;
                        m_vertexBuffer[vertexSize + index].a = m_skeleton->color.a * slot->color.a * attachment->color.a;
                    }

                    for (auto index = 0; index < numIndices; ++index)
                    {
                        m_indexBuffer[indexSize + index] = QuadIndices[index] + vertexSize;
                    }
                }
                else if (type == SP_ATTACHMENT_MESH)
                {
                    auto attachment = SUB_CAST(spMeshAttachment, slot->attachment);
                    currentTexture = SUB_CAST(spAtlasRegion, attachment->rendererObject)->page->rendererObject;

                    numVertices = attachment->super.worldVerticesLength >> 1;
                    numIndices = attachment->trianglesCount;

                    auto vertexSize = m_vertexBuffer.size();
                    auto indexSize = m_indexBuffer.size();

                    m_vertexBuffer.resize(vertexSize + numVertices);
                    m_indexBuffer.resize(indexSize + numIndices);

                    spVertexAttachment_computeWorldVertices(SUPER(attachment), slot, 0, numVertices * sizeof(Vertex) / sizeof(float), (float*)&m_vertexBuffer[vertexSize], 0, sizeof(Vertex) / sizeof(float));

                    for (auto index = 0; index < numVertices; ++index)
                    {
                        m_vertexBuffer[vertexSize + index].u = attachment->uvs[index * 2];
                        m_vertexBuffer[vertexSize + index].v = attachment->uvs[index * 2 + 1];
                        m_vertexBuffer[vertexSize + index].r = m_skeleton->color.r * slot->color.r * attachment->color.r;
                        m_vertexBuffer[vertexSize + index].g = m_skeleton->color.g * slot->color.g * attachment->color.g;
                        m_vertexBuffer[vertexSize + index].b = m_skeleton->color.b * slot->color.b * attachment->color.b;
                        m_vertexBuffer[vertexSize + index].a = m_skeleton->color.a * slot->color.a * attachment->color.a;
                    }

                    for (auto index = 0; index < numIndices; ++index)
                    {
                        m_indexBuffer[indexSize + index] = attachment->triangles[index] + vertexSize;
                    }
                }
                else if (type == SP_ATTACHMENT_BOUNDING_BOX)
                {
                    AZ_Assert(false, "spine render bounding box is not implemented!");
                }
                else if (type == SP_ATTACHMENT_LINKED_MESH)
                {
                    AZ_Assert(false, "spine render linked mesh is not implemented!");
                }
                else if (type == SP_ATTACHMENT_PATH)
                {
                    AZ_Assert(false, "spine render path is not implemented!");
                }
                else if (type == SP_ATTACHMENT_POINT)
                {
                    AZ_Assert(false, "spine render point is not implemented!");
                }
                else if (type == SP_ATTACHMENT_CLIPPING)
                {
//                    AZ_Assert(false, "spine render clipping is not implemented!");
                }

                //first init flush data
                if (flushTexture == nullptr)
                {
                    flushTexture = currentTexture;
                    flushBlendMode = currentBlendMode;
                }

                if (flushBlendMode != currentBlendMode || flushTexture != currentTexture)
                {
                    if (drawIndexCount > 0)
                    {
                        //setmaterial
                        AZStd::shared_ptr<Module::Material> material = GetMaterial(flushBlendMode, (TextureAsset*)flushTexture);
                        if (material)
                        {
                            EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, SetMaterialAndMesh, material, m_contextQueue.size());
                        }

                        Context context;
                        context.BlendMode = flushBlendMode;
                        context.Texture = (TextureAsset*)flushTexture;
                        context.StartIndex = drawStartIndex;
                        context.Count = drawIndexCount;
                        m_contextQueue.push_back(context);
                        drawStartIndex += drawIndexCount;
                        drawIndexCount = 0;
                    }
                    flushBlendMode = currentBlendMode;
                    flushTexture = currentTexture;
                }

                if (numVertices > 0 && numIndices > 0)
                {
                    drawIndexCount += numIndices;
                }
            }
        }

        if (drawIndexCount > 0)
        {
            //setmaterial
            AZStd::shared_ptr<Module::Material> material = GetMaterial(flushBlendMode, (TextureAsset*)flushTexture);
            if (material)
            {
                EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, SetMaterialAndMesh, material, m_contextQueue.size());
            }

            Context context;
            context.BlendMode = flushBlendMode;
            context.Texture = (TextureAsset*)flushTexture;
            context.StartIndex = drawStartIndex;
            context.Count = drawIndexCount;
            m_contextQueue.push_back(context);
        }
    }

 	void SpineComponent::OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world)
    {
        world.StoreToColumnMajorFloat16Ex(m_modelTM);
    }

    void SpineComponent::UpdateMeshBuffer()
    {
        int width, height;
        EBUS_EVENT(WindowSystemRequestBus, GetWindowSize, width, height);

        float proj[16];
        bx::mtxOrtho(proj, 0.0f, float(width), 0.0f, float(height), -100.0f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, nullptr, proj);
        bgfx::setViewMode(0, bgfx::ViewMode::Sequential);

        if (!m_vertexBuffer.empty() && !m_indexBuffer.empty())
        {
            bgfx::updateDynamicVertexBuffer(m_vertexBufferHandle, 0, bgfx::copy(m_vertexBuffer.data(), m_vertexBuffer.size() * sizeof(Vertex)));
            bgfx::updateDynamicIndexBuffer(m_indexBufferHandle, 0, bgfx::copy(m_indexBuffer.data(), m_indexBuffer.size() * sizeof(AZ::u16)));
            m_vertexBuffer.clear();
            m_indexBuffer.clear();
        }
    }

    bool SpineComponent::ApplySubMesh(int index)
    {
        if (!IsValid()) return false;

        bgfx::setTransform(m_modelTM);
        bgfx::setVertexBuffer(0, m_vertexBufferHandle);
        auto& context = m_contextQueue.at(index);
        bgfx::setIndexBuffer(m_indexBufferHandle, context.StartIndex, context.Count);

        return true;
    }
}
