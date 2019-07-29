#include "Renderer/Base/Mesh.h"

#include <AzCore/Asset/AssetManager.h>
#include <AzCore/std/smart_ptr/make_shared.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace Module
{
    Mesh::Mesh(const AZStd::string& relativePath)
    {
        m_config.Create((relativePath + ".xml").c_str(), true);
        AZ::Data::AssetBus::Handler::BusConnect(m_config.GetId());
    }

    Mesh::Mesh(Mesh&& rhv) noexcept
    {
        operator=(AZStd::move(rhv));
    }

    Mesh& Mesh::operator=(Mesh&& rhv) noexcept
    {
        AZ::Data::AssetBus::Handler::operator=(AZStd::move(rhv));

        m_config = rhv.m_config;
        m_subMeshes = rhv.m_subMeshes;
        m_vertexDesc = rhv.m_vertexDesc;
        m_vertices = rhv.m_vertices;
        m_indices = rhv.m_indices;
        m_isShared = rhv.m_isShared;
        m_isReady = rhv.m_isReady;
        m_isDirty = rhv.m_isDirty;
        m_isDynamic = rhv.m_isDynamic;

        if (m_isDynamic)
        {
            m_dynamic = rhv.m_dynamic;
            rhv.m_dynamic.m_vertexHandle = BGFX_INVALID_HANDLE;
            rhv.m_dynamic.m_indexHandle = BGFX_INVALID_HANDLE;
        }
        else
        {
            m_static = rhv.m_static;
            rhv.m_static.m_vertexHandle = BGFX_INVALID_HANDLE;
            rhv.m_static.m_indexHandle = BGFX_INVALID_HANDLE;
        }

        return *this;
    }

    Mesh::~Mesh()
    {
        if (m_isDynamic)
        {
            if (bgfx::isValid(m_dynamic.m_vertexHandle))
            {
                bgfx::destroy(m_dynamic.m_vertexHandle);
            }
            if (bgfx::isValid(m_dynamic.m_indexHandle))
            {
                bgfx::destroy(m_dynamic.m_indexHandle);
            }
        }
        else
        {
            if (bgfx::isValid(m_static.m_vertexHandle))
            {
                bgfx::destroy(m_static.m_vertexHandle);
            }
            if (bgfx::isValid(m_static.m_indexHandle))
            {
                bgfx::destroy(m_static.m_indexHandle);
            }
        }
    }

    void Mesh::Reflect(AZ::ReflectContext* context)
    {
        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<Mesh>()
                ;
        }
    }

    void Mesh::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        AZ::Data::AssetBus::Handler::BusDisconnect(asset.GetId()); 

        auto* meshAsset = m_config.Get();

        m_subMeshes = meshAsset->m_subMeshes;
        m_vertexDesc = meshAsset->m_vertexDesc;
        m_vertices = meshAsset->m_vertexBuffer;
        m_indices = meshAsset->m_indices;

        m_isReady = false;
        m_isDirty = false;
        m_isDynamic = false;

        m_aabb = meshAsset->m_aabb;

        m_config.Release();
    }

    void Mesh::MarkUnique()
    {
        m_isShared = false;
    }

    void Mesh::MarkDynamic()
    {
        if (m_isDynamic)
        {
            return;
        }
        if (bgfx::isValid(m_static.m_vertexHandle))
        {
            bgfx::destroy(m_static.m_vertexHandle);
        }
        if (bgfx::isValid(m_static.m_indexHandle))
        {
            bgfx::destroy(m_static.m_indexHandle);
        }
        m_isReady = false;
        m_isDirty = false;
        m_isDynamic = true;
    }

    void Mesh::ApplySubMesh(size_t index)
    {
        AZ_Assert(index < m_subMeshes.size(), "Sub mesh out of bounds\n");
        if (!m_isReady)
        {
            m_isReady = true;
            if (m_isDynamic)
            {
                m_dynamic.m_vertexHandle = bgfx::createDynamicVertexBuffer(bgfx::copy(m_vertices.data(), m_vertices.size()), m_vertexDesc);
                m_dynamic.m_indexHandle = bgfx::createDynamicIndexBuffer(bgfx::copy(m_indices.data(), m_indices.size()));
            }
            else
            {
                m_static.m_vertexHandle = bgfx::createVertexBuffer(bgfx::copy(m_vertices.data(), m_vertices.size()), m_vertexDesc);
                m_static.m_indexHandle = bgfx::createIndexBuffer(bgfx::copy(m_indices.data(), m_indices.size()));
            }
        }
        if (m_isDynamic)
        {
            if (m_isDirty)
            {
                AZ_Assert(bgfx::isValid(m_dynamic.m_vertexHandle) && bgfx::isValid(m_dynamic.m_indexHandle), "Dynamic handle is not valid!\n");
                bgfx::updateDynamicVertexBuffer(m_dynamic.m_vertexHandle, 0, bgfx::copy(m_vertices.data(), m_vertices.size())); // TODO only update dirty part
                bgfx::updateDynamicIndexBuffer(m_dynamic.m_indexHandle, 0, bgfx::copy(m_indices.data(), m_indices.size())); // TODO only update dirty part
            }
            bgfx::setVertexBuffer(0, m_dynamic.m_vertexHandle);
            bgfx::setIndexBuffer(m_dynamic.m_indexHandle, m_subMeshes[index].m_firstIndex, m_subMeshes[index].m_indexCount);
        }
        else
        {
            bgfx::setVertexBuffer(0, m_static.m_vertexHandle);
            bgfx::setIndexBuffer(m_static.m_indexHandle, m_subMeshes[index].m_firstIndex, m_subMeshes[index].m_indexCount);
        }
    }

    MeshPtr Mesh::Clone()
    {
        auto mesh = AZStd::make_shared<Mesh>();
        // TODO
        return mesh;
    }
}
