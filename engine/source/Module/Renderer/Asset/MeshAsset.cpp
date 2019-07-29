#include "Renderer/Asset/MeshAsset.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace Module
{
    void MeshAsset::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SubMesh>()
                ->Field("firstIndex", &SubMesh::m_firstIndex)
                ->Field("indexCount", &SubMesh::m_indexCount)
                ;

            serializeContext->Class<MeshAsset>()
                ->Field("name", &MeshAsset::m_name)
                ->Field("position", &MeshAsset::m_position)
                ->Field("normal", &MeshAsset::m_normal)
                ->Field("color", &MeshAsset::m_color)
                ->Field("texcoord0", &MeshAsset::m_texcoord0)
                ->Field("texcoord1", &MeshAsset::m_texcoord1)
                ->Field("tangent", &MeshAsset::m_tangent)
                ->Field("indices", &MeshAsset::m_indices)
                ->Field("subMeshes", &MeshAsset::m_subMeshes)
                ->Field("bounds", &MeshAsset::m_aabb)
                ;
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<SubMesh>("SubMesh")
                ->Constructor()
                ->Property("firstIndex", BehaviorValueProperty(&SubMesh::m_firstIndex))
                ->Property("indexCount", BehaviorValueProperty(&SubMesh::m_indexCount))
                ;

            behaviorContext->Class<MeshAsset>("MeshAsset")
                ->Constructor()
                ->Property("name", BehaviorValueProperty(&MeshAsset::m_name))
                ->Property("position", BehaviorValueProperty(&MeshAsset::m_position))
                ->Property("normal", BehaviorValueProperty(&MeshAsset::m_normal))
                ->Property("color", BehaviorValueProperty(&MeshAsset::m_color))
                ->Property("texcoord0", BehaviorValueProperty(&MeshAsset::m_texcoord0))
                ->Property("texcoord1", BehaviorValueProperty(&MeshAsset::m_texcoord1))
                ->Property("tangent", BehaviorValueProperty(&MeshAsset::m_tangent))
                ->Property("indices", BehaviorValueProperty(&MeshAsset::m_indices))
                ->Property("subMeshes", BehaviorValueProperty(&MeshAsset::m_subMeshes))
                ->Property("bounds", BehaviorValueProperty(&MeshAsset::m_aabb))
                ;
        }
    }

    bool MeshAssetHandler::LoadAssetData(const AZ::Data::Asset<AZ::Data::AssetData>& asset, AZ::IO::GenericStream* stream, const AZ::Data::AssetFilterCB& assetLoadFilterCB)
    {
        if (AZ::GenericAssetHandler<MeshAsset>::LoadAssetData(asset, stream, assetLoadFilterCB))
        {
            auto* meshAsset = asset.GetAs<MeshAsset>();
            auto& vertexDesc = meshAsset->m_vertexDesc;
            auto& vertexBuffer = meshAsset->m_vertexBuffer;
            auto& position = meshAsset->m_position;
            auto& normal = meshAsset->m_normal;
            auto& color = meshAsset->m_color;
            auto& texcoord0 = meshAsset->m_texcoord0;
            auto& texcoord1 = meshAsset->m_texcoord1;
            auto& tangent = meshAsset->m_tangent;

            const auto vertexCount = position.size();
            AZ_Assert(vertexCount > 0, "Invalid mesh!\n");

            vertexDesc.begin(bgfx::getRendererType());
            if (!position.empty())
            {
                AZ_Assert(vertexCount < position.size(), "Invalid mesh, not enough position data!\n");
                vertexDesc.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);
            }
            if (!normal.empty())
            {
                AZ_Assert(vertexCount < normal.size(), "Invalid mesh, not enough normal data!\n");
                vertexDesc.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float);
            }
            if (!color.empty())
            {
                AZ_Assert(vertexCount < color.size(), "Invalid mesh, not enough color data!\n");
                vertexDesc.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float);
            }
            if (!texcoord0.empty())
            {
                AZ_Assert(vertexCount < texcoord0.size(), "Invalid mesh, not enough texcoord0 data!\n");
                vertexDesc.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
            }
            if (!texcoord1.empty())
            {
                AZ_Assert(vertexCount < texcoord1.size(), "Invalid mesh, not enough texcoord1 data!\n");
                vertexDesc.add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float);
            }
            if (!tangent.empty())
            {
                AZ_Assert(vertexCount < tangent.size(), "Invalid mesh, not enough tangent data!\n");
                vertexDesc.add(bgfx::Attrib::Tangent, 4, bgfx::AttribType::Float);
            }
            vertexDesc.end();

            vertexBuffer.resize(vertexDesc.getSize(position.size()));

            for (size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
            {
                if (!position.empty())
                {
                    position[vertexCount].StoreToFloat3(reinterpret_cast<float*>(vertexBuffer.data() + vertexDesc.getStride() * vertexIndex + vertexDesc.getOffset(bgfx::Attrib::Position)));
                }
                if (!normal.empty())
                {
                    normal[vertexCount].StoreToFloat3(reinterpret_cast<float*>(vertexBuffer.data() + vertexDesc.getStride() * vertexIndex + vertexDesc.getOffset(bgfx::Attrib::Normal)));
                }
                if (!color.empty())
                {
                    color[vertexCount].StoreToFloat4(reinterpret_cast<float*>(vertexBuffer.data() + vertexDesc.getStride() * vertexIndex + vertexDesc.getOffset(bgfx::Attrib::Color0)));
                }
                if (!texcoord0.empty())
                {
                    texcoord0[vertexCount].StoreToFloat2(reinterpret_cast<float*>(vertexBuffer.data() + vertexDesc.getStride() * vertexIndex + vertexDesc.getOffset(bgfx::Attrib::TexCoord0)));
                }
                if (!texcoord1.empty())
                {
                    texcoord1[vertexCount].StoreToFloat2(reinterpret_cast<float*>(vertexBuffer.data() + vertexDesc.getStride() * vertexIndex + vertexDesc.getOffset(bgfx::Attrib::TexCoord1)));
                }
                if (!tangent.empty())
                {
                    tangent[vertexCount].StoreToFloat4(reinterpret_cast<float*>(vertexBuffer.data() + vertexDesc.getStride() * vertexIndex + vertexDesc.getOffset(bgfx::Attrib::Tangent)));
                }
            }

            return true;
        }
        return false;
    }
}

namespace AZ
{
    AZ_TYPE_INFO_SPECIALIZE(bgfx::VertexDecl, "{89F9A719-F455-4015-A3B2-533783FDAF7C}");
    AZ_TYPE_INFO_SPECIALIZE(Module::MeshAsset::SubMesh, "{9AF2C57D-2BDA-421B-8EFF-7F3671B7F86A}");
}
