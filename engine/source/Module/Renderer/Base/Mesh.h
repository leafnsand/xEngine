#pragma once

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Math/Vector2.h>
#include <AzCore/Math/Aabb.h>

#include "Renderer/Asset/MeshAsset.h"

#include <bgfx/bgfx.h>

namespace Module
{
    class Mesh;

    using MeshPtr = AZStd::shared_ptr<Mesh>;

    class Mesh : public AZ::Data::AssetBus::Handler
    {
    public:
        AZ_CLASS_ALLOCATOR(Mesh, AZ::SystemAllocator, 0);
        AZ_RTTI(Mesh, "{12B6EF31-2282-4C05-83F3-69365D40058B}");

        Mesh() {}
        explicit Mesh(const AZStd::string& relativePath);

        // non-copyable
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        // moveable
        Mesh(Mesh&&) noexcept;
        Mesh& operator=(Mesh&&) noexcept;

        ~Mesh() override;

        static void Reflect(AZ::ReflectContext* context);

        // AZ::Data::AssetBus::Handler
        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;

        bool IsShared() const { return m_isShared; }

        void MarkUnique();
        void MarkDynamic();

        void ApplySubMesh(size_t index);

        MeshPtr Clone();

    private:
        AZ::Data::Asset<MeshAsset>            m_config;
        
        AZStd::vector<MeshAsset::SubMesh>     m_subMeshes;

        bgfx::VertexDecl                      m_vertexDesc;

        AZStd::vector<char>                   m_vertices;
        AZStd::vector<AZ::u16>                m_indices;

        bool                                  m_isShared  = true;
        bool                                  m_isReady   = false;
        bool                                  m_isDirty   = false;
        bool                                  m_isDynamic = false;

        AZ::Aabb                              m_aabb;

        union
        {
            struct
            {
                bgfx::VertexBufferHandle m_vertexHandle = BGFX_INVALID_HANDLE;
                bgfx::IndexBufferHandle  m_indexHandle  = BGFX_INVALID_HANDLE;
            } m_static = { BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE };
            struct
            {
                bgfx::DynamicVertexBufferHandle m_vertexHandle = BGFX_INVALID_HANDLE;
                bgfx::DynamicIndexBufferHandle  m_indexHandle  = BGFX_INVALID_HANDLE;
            } m_dynamic;
        };
    };
}
