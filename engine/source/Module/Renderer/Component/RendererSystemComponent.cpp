#include "Renderer/Component/RendererSystemComponent.h"
#include "Renderer/Component/RendererComponent.h"
#include "Renderer/Component/CameraComponent.h"

#include "Renderer/Asset/MeshAsset.h"
#include "Renderer/Asset/ShaderAsset.h"
#include "Renderer/Asset/TextureAssetHandler.h"

#include "Renderer/Base/Shader.h"
#include "Renderer/Base/Material.h"

#include "Renderer/Util/MeshUtil.h"

#include "Window/EBus/WindowSystemComponentBus.h"

#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/StringFunc/StringFunc.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/std/smart_ptr/make_shared.h>
#include <AzCore/std/sort.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

namespace Module
{
    void RendererSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        TextureAsset::Reflect(context);
        MeshAsset::Reflect(context);
        ShaderAsset::Reflect(context);
        MaterialAsset::Reflect(context);

        Material::Reflect(context);
        Mesh::Reflect(context);
        Sprite::Reflect(context);
        Texture::Reflect(context);

        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<RendererSystemComponent, AZ::Component>()
                ->Field("resetFlags", &RendererSystemComponent::m_resetFlags);
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<RendererSystemComponent>("RendererSystemComponent")
                ->Constructor()
                ->Property("resetFlags", BehaviorValueProperty(&RendererSystemComponent::m_resetFlags));

            behaviorContext->EBus<RendererSystemRequestBus>("RendererSystemRequestBus")
                ->Event("SetResetFlags", &RendererSystemRequestBus::Events::SetResetFlags);
        }
    }

    RendererSystemComponent::RendererSystemComponent()
    {
    }

    void RendererSystemComponent::Init()
    {
    }

    void RendererSystemComponent::Activate()
    {
        m_assetHandlers.emplace_back(aznew TextureAssetHandler);
        m_assetHandlers.emplace_back(aznew MaterialAssetHandler);
        m_assetHandlers.emplace_back(aznew ShaderAssetHandler);
        m_assetHandlers.emplace_back(aznew MeshAssetHandler);

        bgfx::PlatformData data = {};
        EBUS_EVENT_RESULT(data.nwh, WindowSystemRequestBus, GetNativeWindowHandle);
        bgfx::setPlatformData(data);

        int width, height;
        EBUS_EVENT(WindowSystemRequestBus, GetWindowSize, width, height);

        bgfx::init();
        bgfx::reset(width, height
#if defined(AZ_PLATFORM_WINDOWS)
            , m_resetFlags
#endif
        );
        
#if defined(AZ_ENABLE_TRACING)
        bgfx::setDebug(BGFX_DEBUG_TEXT);
#endif

        AZ::SystemTickBus::Handler::BusConnect();
        RendererSystemRequestBus::Handler::BusConnect();
        WindowsSystemNotificationBus::Handler::BusConnect();
    }

    void RendererSystemComponent::Deactivate()
    {
        m_sprites.clear();
        m_shaders.clear();
        m_materials.clear();
        m_meshes.clear();
        m_assetHandlers.clear();

        AZ::SystemTickBus::Handler::BusDisconnect();
        RendererSystemRequestBus::Handler::BusDisconnect();
        WindowsSystemNotificationBus::Handler::BusDisconnect();

        bgfx::shutdown();
    }

    void RendererSystemComponent::OnSystemTick()
    {
        AZStd::vector<CameraComponent*> cameras;

        CameraRequestBus::EnumerateHandlers([&](CameraRequest* cameraHandler)
        {
            cameras.push_back(azrtti_cast<CameraComponent*>(cameraHandler));

            return true;
        });

        AZStd::sort(cameras.begin(), cameras.end(), [](CameraComponent* lhv, CameraComponent* rhv)
        {
            return lhv->m_depth < rhv->m_depth;
        });

        bgfx::ViewId currentViewId = 0;

        for (auto camera : cameras)
        {
            AZ::Transform cameraWorldTM;
            EBUS_EVENT_ID_RESULT(cameraWorldTM, camera->GetEntityId(), AZ::TransformBus, GetWorldTM);

            camera->ResetView(currentViewId);

            AZStd::vector<RenderNode> renderNodes;

            RendererRequestBus::EnumerateHandlers([&](RendererRequest* rendererHandler)
            {
                auto* renderer = azrtti_cast<RendererComponent*>(rendererHandler);

                AZ::Transform rnWorldTM;
                EBUS_EVENT_ID_RESULT(rnWorldTM, renderer->GetEntityId(), AZ::TransformBus, GetWorldTM);

                float distance = cameraWorldTM.GetPosition().GetDistance(rnWorldTM.GetPosition());

                /* TODO cull render node
                if (camera->CullRenderNode(renderer, mesh)) return;
                */

                // TODO check pass valid or not
                for (size_t materialIndex = 0; materialIndex < renderer->GetMaterialCount(); ++materialIndex)
                {
                    auto material = renderer->GetMaterial(materialIndex);
                    auto shader = material->m_shader;

                    for (size_t passIndex = 0; passIndex < shader->m_passes.size(); ++passIndex)
                    {
                        renderNodes.emplace_back(renderer,
                                                 materialIndex,
                                                 material.get(),
                                                 shader.get(),
                                                 &shader->m_passes.at(passIndex),
                                                 distance,
                                                 rnWorldTM,
                                                 renderer->GetSortingLayer(),
                                                 renderer->GetOrderInLayer());
                    }
                }

                return true;
            });

            AZStd::sort(renderNodes.begin(), renderNodes.end());

            for (auto& rn : renderNodes)
            {
                rn.Apply(currentViewId);
            }

            camera->DrawSkybox(currentViewId);

            ++currentViewId;
        }

#if defined(AZ_ENABLE_TRACING)
        float delta = 0.0f;
        EBUS_EVENT_RESULT(delta, AZ::TickRequestBus, GetTickDeltaTime);
        const auto stat = bgfx::getStats();
        bgfx::dbgTextPrintf(0, 0, 0x0F, "FPS: %.2f DC: %d", 1.0f / delta, stat->numDraw);
#endif

        bgfx::frame();
    }

    void RendererSystemComponent::SetResetFlags(uint32_t resetFlags)
    {
        m_resetFlags = resetFlags;
    }

    TexturePtr RendererSystemComponent::GetTexture(const AZStd::string& path)
    {
        const auto caps = bgfx::getCaps();
        AZStd::string replacedPath = path;

        if (caps->formats[bgfx::TextureFormat::BC3] != BGFX_CAPS_FORMAT_TEXTURE_NONE)
        {
            AZ::StringFunc::Path::ReplaceExtension(replacedPath, "dds");
        }
        else if (caps->formats[bgfx::TextureFormat::PTC14] != BGFX_CAPS_FORMAT_TEXTURE_NONE)
        {
            AZ::StringFunc::Path::ReplaceExtension(replacedPath, "pvr");
        }
        else if (caps->formats[bgfx::TextureFormat::ETC1] != BGFX_CAPS_FORMAT_TEXTURE_NONE)
        {
            AZ::StringFunc::Path::ReplaceExtension(replacedPath, "ktx");
        }
        else // we currently only support compressed texture
        {
            AZ_Warning("RendererSystemComponent", false, "Platform do not support any valid texture format!\n");
        }

        TexturePtr texture;
        const auto iterator = m_textures.find(path);
        if (iterator == m_textures.end() || iterator->second.expired())
        {
            texture = AZStd::make_shared<Texture>(replacedPath);
            m_textures[path] = texture;
        }
        else
        {
            texture = iterator->second.lock();
        }
        return texture;
    }

    ShaderPtr RendererSystemComponent::GetShader(const AZStd::string& path)
    {
        ShaderPtr shader;
        const auto iterator = m_shaders.find(path);
        if (iterator == m_shaders.end() || iterator->second.expired())
        {
            shader = AZStd::make_shared<Shader>(path);
            m_shaders[path] = shader;
        }
        else
        {
            shader = iterator->second.lock();
        }
        return shader;
    }

    MaterialPtr RendererSystemComponent::GetMaterial(const AZStd::string& path)
    {
        MaterialPtr material;
        const auto iterator = m_materials.find(path);
        if (iterator == m_materials.end() || iterator->second.expired())
        {
            material = AZStd::make_shared<Material>(path);
            m_materials[path] = material;
        }
        else
        {
            material = iterator->second.lock();
        }
        return material;
    }

    MeshPtr RendererSystemComponent::GetMesh(AZStd::string& path)
    {
        MeshPtr mesh;
        const auto iterator = m_meshes.find(path);
        if (iterator == m_meshes.end() || iterator->second.expired())
        {
            mesh = AZStd::make_shared<Mesh>(path);
            m_meshes[path] = mesh;
        }
        else
        {
            mesh = iterator->second.lock();
        }
        return mesh;
    }

    SpritePtr RendererSystemComponent::GetSprite(const AZStd::string& path, const AZStd::string& spriteName)
    {
        SpritePtr sprite;
        const auto iterator = m_sprites.find(path);
        if (iterator == m_sprites.end() || iterator->second.expired())
        {
            sprite = AZStd::make_shared<Sprite>(path, spriteName);
            m_sprites[path] = sprite;
        }
        else
        {
            sprite = iterator->second.lock();
        }
        return sprite;
    }

    void RendererSystemComponent::OnWindowSizeChanged(int width, int height)
    {
        bgfx::reset(width, height
#if defined(AZ_PLATFORM_WINDOWS)
            , m_resetFlags
#endif
        );
    }
}
