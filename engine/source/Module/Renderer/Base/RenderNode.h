#pragma once

#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Math/Transform.h>

#include <bgfx/bgfx.h>

namespace Module
{
    class RendererComponent;
    class Material;
    class Shader;
    class Pass;

    class RenderNode
    {
    public:
        AZ_CLASS_ALLOCATOR(RenderNode, AZ::SystemAllocator, 0);

        RenderNode(RendererComponent* renderer,
                   size_t materialIndex,
                   Material* material,
                   Shader* shader,
                   Pass* pass,
                   float distance,
                   AZ::Transform& transform,
                   AZ::s16 sortingLayer,
                   AZ::s16 orderInLayer)
            : m_renderer(renderer)
            , m_materialIndex(materialIndex)
            , m_material(material)
            , m_shader(shader)
            , m_pass(pass)
            , m_distance(distance)
            , m_sortingLayer(sortingLayer)
            , m_orderInLayer(orderInLayer)
        {
            transform.StoreToColumnMajorFloat16Ex(m_worldMatrix);
        }

        void Apply(bgfx::ViewId viewId);

    private:
        RendererComponent* m_renderer        = nullptr;
        size_t             m_materialIndex   = 0;

        Material*          m_material        = nullptr;
        Shader*            m_shader          = nullptr;
        Pass*              m_pass            = nullptr;

        float              m_distance        = 0.0f;
        float              m_worldMatrix[16] = {};

        AZ::s16            m_sortingLayer    = 0;
        AZ::s16            m_orderInLayer    = 0;

        friend bool operator<(const RenderNode& lhv, const RenderNode& rhv);
    };
}
