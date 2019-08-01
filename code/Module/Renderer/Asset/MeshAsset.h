#pragma once

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Asset/GenericAssetHandler.h>
#include <AzCore/std/containers/vector.h>
#include <AzCore/Math/Color.h>
#include <AzCore/Math/Vector2.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Math/Aabb.h>

#include <bgfx/bgfx.h>

namespace Module
{
    class MeshAsset : public AZ::Data::AssetData
    {
    public:
        AZ_CLASS_ALLOCATOR(MeshAsset, AZ::SystemAllocator, 0);
        AZ_RTTI(MeshAsset, "{17166E9D-C8BF-4231-B7B1-63BBF07623D7}", AZ::Data::AssetData);

        struct SubMesh
        {
            AZ_CLASS_ALLOCATOR(SubMesh, AZ::SystemAllocator, 0);

            AZ::u16 m_firstIndex = 0;
            AZ::u16 m_indexCount = 0;
        };

        static void Reflect(AZ::ReflectContext* context);

    private:
        AZStd::string m_name;

        AZStd::vector<AZ::Vector3> m_position;
        AZStd::vector<AZ::Vector3> m_normal;
        AZStd::vector<AZ::Color>   m_color;
        AZStd::vector<AZ::Vector2> m_texcoord0;
        AZStd::vector<AZ::Vector2> m_texcoord1;
        AZStd::vector<AZ::Vector4> m_tangent;

        AZStd::vector<AZ::u16> m_indices;

        AZStd::vector<SubMesh> m_subMeshes;

        AZ::Aabb m_aabb;

        bgfx::VertexDecl m_vertexDesc;

        AZStd::vector<char> m_vertexBuffer;

        friend class Mesh;
        friend class MeshAssetHandler;
    };

    class MeshAssetHandler : public AZ::GenericAssetHandler<MeshAsset>
    {
    public:
        bool LoadAssetData(const AZ::Data::Asset<AZ::Data::AssetData>& asset, AZ::IO::GenericStream* stream, const AZ::Data::AssetFilterCB& assetLoadFilterCB) override;
    };
}
