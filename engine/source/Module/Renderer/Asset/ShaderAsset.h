#pragma once

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Asset/GenericAssetHandler.h>

namespace Module
{
    class ShaderAsset : public AZ::Data::AssetData
    {
    public:
        AZ_CLASS_ALLOCATOR(ShaderAsset, AZ::SystemAllocator, 0);
        AZ_RTTI(ShaderAsset, "{460ECC3C-BE55-4D9B-AD9C-DCFFE7CE4965}", AZ::Data::AssetData);

        struct Pass
        {
            AZ_CLASS_ALLOCATOR(Pass, AZ::SystemAllocator, 0);

            AZStd::string m_name;
            AZStd::string m_vs;
            AZStd::string m_fs;
            AZ::u64       m_rs = 0;
        };

        static void Reflect(AZ::ReflectContext* context);

    private:
        AZStd::string       m_name;
        AZStd::vector<Pass> m_passes;

        friend class Shader;
    };

    using ShaderAssetHandler = AZ::GenericAssetHandler<ShaderAsset>;
}
