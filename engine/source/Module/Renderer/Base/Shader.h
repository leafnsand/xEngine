#pragma once

#include "Renderer/Base/Pass.h"
#include "Renderer/Asset/ShaderAsset.h"

#include <AzCore/Asset/AssetCommon.h>

#include <bgfx/bgfx.h>

namespace Module
{
    class Shader
        : public AZStd::intrusive_list_node<Shader>
        , public AZ::Data::AssetBus::Handler
    {
    public:
        AZ_CLASS_ALLOCATOR(Shader, AZ::SystemAllocator, 0);

        explicit Shader(const AZStd::string& relativePath);

        // non-copyable
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        // AZ::Data::AssetBus::Handler
        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;

        void SetReverseCull(bool flag);

        bool IsValid() const;

    private:
        AZ::Data::Asset<ShaderAsset> m_config;
        AZStd::vector<Pass>          m_passes;

        friend class RendererSystemComponent;
    };

    using ShaderPtr = AZStd::shared_ptr<Shader>;
}