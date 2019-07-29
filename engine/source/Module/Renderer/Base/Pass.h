#pragma once

#include "Renderer/Asset/ShaderAsset.h"

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Asset/BinaryAssetHandler.h>

#include <bgfx/bgfx.h>

namespace Module
{
    class Pass : public AZ::Data::AssetBus::MultiHandler
    {
    public:
        AZ_CLASS_ALLOCATOR(Pass, AZ::SystemAllocator, 0);

        explicit Pass(const ShaderAsset::Pass& config);

        // non-copyable
        Pass(const Pass&) = delete;
        Pass& operator=(const Pass&) = delete;

        Pass(Pass&&) noexcept;
        Pass& operator=(Pass&&) noexcept;

        ~Pass();

        // AZ::Data::AssetBus::MultiHandler
        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;

        void SetReverseCull(bool value);

        bool IsValid() const { return bgfx::isValid(m_program); }

        void Apply(bgfx::ViewId viewId) const;

    private:
        AZ::Data::Asset<AZ::BinaryAsset> m_vs;
        AZ::Data::Asset<AZ::BinaryAsset> m_fs;

        bgfx::ProgramHandle              m_program   = BGFX_INVALID_HANDLE;
        AZ::u64                          m_defaultRs = 0;
        AZ::u64                          m_rs        = 0;
    };
}