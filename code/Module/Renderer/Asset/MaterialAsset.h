#pragma once

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Asset/GenericAssetHandler.h>
#include <AzCore/Math/Vector4.h>
#include <AzCore/RTTI/ReflectContext.h>

namespace Module
{
    class MaterialAsset : public AZ::Data::AssetData
    {
    public:
        AZ_CLASS_ALLOCATOR(MaterialAsset, AZ::SystemAllocator, 0);
        AZ_RTTI(MaterialAsset, "{2470DC12-E17E-4A67-B47E-170456BAFEB8}", AZ::Data::AssetData);

        struct Property
        {
            AZ_CLASS_ALLOCATOR(Property, AZ::SystemAllocator, 0);

            enum class Type : AZ::u8
            {
                Vector,
                Texture2D,
                Texture3D,
                TextureCube,
            };

            AZStd::string m_name;
            Type          m_type = Type::Vector;
            AZ::Vector4   m_vectorValue;
            AZStd::string m_textureValue;
        };

        static void Reflect(AZ::ReflectContext* context);

    private:
        AZStd::string           m_name;
        AZStd::string           m_shader;
        AZ::s32                 m_queue = 0;
        AZStd::vector<Property> m_properties;

        friend class Material;
    };

    using MaterialAssetHandler = AZ::GenericAssetHandler<MaterialAsset>;
}
