#include "Renderer/Base/RenderNode.h"
#include "Renderer/Base/Material.h"
#include "Renderer/Component/RendererComponent.h"

namespace Module
{
    void RenderNode::Apply(bgfx::ViewId viewId)
    {
        m_renderer->Render(m_materialIndex); // set vertex buffer
        bgfx::setTransform(m_worldMatrix);   // set uniform
        m_material->Apply();                 // set uniform
        m_pass->Apply(viewId);               // set state, set shader, submit drawcall
    }

    bool operator<(const RenderNode& lhv, const RenderNode& rhv)
    {
        if (lhv.m_sortingLayer != rhv.m_sortingLayer)
        {
            return lhv.m_sortingLayer < rhv.m_sortingLayer;
        }
        if (lhv.m_orderInLayer != rhv.m_orderInLayer)
        {
            return lhv.m_orderInLayer < rhv.m_orderInLayer;
        }
        if (lhv.m_material->GetQueue() != rhv.m_material->GetQueue())
        {
            return lhv.m_material->GetQueue() < rhv.m_material->GetQueue();
        }
        if (lhv.m_distance != rhv.m_distance)
        {
            return lhv.m_distance < rhv.m_distance;
        }
        // is first pass
        // lightmap index
        // static batch index
        if (lhv.m_materialIndex != rhv.m_materialIndex)
        {
            return lhv.m_materialIndex < rhv.m_materialIndex;
        }
        if (lhv.m_shader != rhv.m_shader)
        {
            return lhv.m_shader < rhv.m_shader;
        }
        if (lhv.m_material != rhv.m_material)
        {
            return lhv.m_material < rhv.m_material;
        }
        if (lhv.m_pass != rhv.m_pass)
        {
            return lhv.m_pass < rhv.m_pass;
        }
        // fall back
        return &lhv < &rhv;
    }
}
