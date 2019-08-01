#include "Gfx/Component/CanvasComponent.h"
#include "Window/EBus/WindowSystemComponentBus.h"
#include "Renderer/EBus/RendererComponentBus.h"
#include "Renderer/Asset/TextureAsset.h"
#include "Renderer/Base/Material.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/StringFunc/StringFunc.h>

#include "AzCore/Math/Vector4.h"
#include "AzCore/Math/Matrix4x4.h"

#include <bx/math.h>

namespace Module
{
    static const uint16_t QuadIndices[] = { 0, 1, 2, 0, 2, 3 };

    static AZStd::shared_ptr<Material> GetMaterial(GfxMaterialType gfxMaterialType, TextureAsset* texture)
    {
        AZStd::shared_ptr<Material> material = nullptr;
        switch (gfxMaterialType)
        {
        case V2F_C4F:
            EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "gfx_col");
            if (material == nullptr)
            {
                EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "gfx_col", "gfx_col");
            }
            break;
        case V2F_T2F_C4F:
            if (texture == nullptr)
            {
                return nullptr;
            }
            int index = texture->GetHandle().idx;
            AZStd::string ext = AZStd::string::format("_%d", index);
            EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, GetUnquieMaterial, "gfx_tex" + ext);
            if (material == nullptr)
            {
                EBUS_EVENT_RESULT(material, Module::RendererSystemRequestBus, CreateUnquieMaterial, "gfx_tex", "gfx_tex" + ext);
                if (material)
                {
                    Material::Property property;
                    property.m_type = Material::Property::Type::Texture;
                    texture->Acquire();
                    property.m_textureValue = texture;
                    material->SetProperty(property);
                }
            }
            break;
        }
        return material;
    }

    inline int calculatePiceses(float r)
    {
        int piceses = (int)sqrtf(r * 20.0f);
        return AZ::GetMax<int>(piceses, 8);
    }

    void CanvasComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<CanvasComponent, AZ::Component>()
                ->Field("name", &CanvasComponent::m_name)
                ;
        }
    }

    void CanvasComponent::Activate()
    {
        bgfx::VertexDecl vertexDecl;
        vertexDecl
            .begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
            .end();

        m_vertexBufferHandle = bgfx::createDynamicVertexBuffer(std::numeric_limits<AZ::u16>::max(), vertexDecl);
        m_indexBufferHandle = bgfx::createDynamicIndexBuffer(std::numeric_limits<AZ::u16>::max());

        CanvasRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());
        // RendererSystemNotificationBus::Handler::BusConnect();
        MeshProviderRequestBus::Handler::BusConnect(GetEntityId());
        // AZ::TickBus::Handler::BusConnect();
    }

    void CanvasComponent::Deactivate()
    {
        CanvasRequestBus::Handler::BusDisconnect();
        AZ::TransformNotificationBus::Handler::BusDisconnect();
        // RendererSystemNotificationBus::Handler::BusDisconnect();
        MeshProviderRequestBus::Handler::BusDisconnect();
        // AZ::TickBus::Handler::BusDisconnect();

        bgfx::destroy(m_vertexBufferHandle);
        bgfx::destroy(m_indexBufferHandle);
    }

    // AZ::TransformBus::Handler
    void CanvasComponent::OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world)
    {
        world.StoreToColumnMajorFloat16Ex(m_modelTM);
    }

    // CanvasRequestBus::Handler
    bool CanvasComponent::IsValid() const
    {
        return true;
    }

    void CanvasComponent::DrawImage(TextureAsset* image, AZ::Vector4& rect, AZ::Matrix4x4& transform, const AZ::Color& color)
    {
        static const auto vertexCount = 4;
        static const auto indexCount = 6;

        if (!image->IsReady())
        {
            return;
        }

        AZ::Vector2 size(image->GetInfo().width, image->GetInfo().height);

        if (rect.GetX() < 0.0f) rect.SetX(0.0f);
        if (rect.GetY() < 0.0f) rect.SetY(0.0f);
        if (rect.GetZ() < 0.0f) rect.SetZ(size.GetX());
        if (rect.GetW() < 0.0f) rect.SetW(size.GetY());

        rect.SetX(AZStd::clamp((float)rect.GetX(), 0.0f, size.GetX()));
        rect.SetY(AZStd::clamp((float)rect.GetY(), 0.0f, size.GetY()));
        rect.SetZ(AZStd::clamp((float)rect.GetZ(), 0.0f, size.GetX() - (float)rect.GetX()));
        rect.SetW(AZStd::clamp((float)rect.GetW(), 0.0f, size.GetY() - (float)rect.GetY()));

        const auto top_left = AZ::Vector4(0.0, 0.0, 0.0, 1.0) * transform;
        const auto top_right = AZ::Vector4(rect.GetZ(), 0.0, 0.0, 1.0)* transform;
        const auto bottom_left = AZ::Vector4(rect.GetZ(), rect.GetW(), 0.0, 1.0)* transform;
        const auto bottom_right = AZ::Vector4(0.0, rect.GetW(), 0.0, 1.0)* transform;

        const AZ::Vector2 min(rect.GetX() / (AZ::VectorFloat)size.GetX(), rect.GetY() / (const AZ::VectorFloat)size.GetY());
        const AZ::Vector2 max(min.GetX() + rect.GetZ() / (AZ::VectorFloat)size.GetX(), min.GetY() + rect.GetW() / (AZ::VectorFloat)size.GetY());

        m_vertexBuffer.push_back(VertexData(top_left.GetX(), top_left.GetY(), min.GetX(), max.GetY(), color));
        m_vertexBuffer.push_back(VertexData(top_right.GetX(), top_right.GetY(), max.GetX(), max.GetY(), color));
        m_vertexBuffer.push_back(VertexData(bottom_left.GetX(), bottom_left.GetY(), max.GetX(), min.GetY(), color));
        m_vertexBuffer.push_back(VertexData(bottom_right.GetX(), bottom_right.GetY(), min.GetX(), min.GetY(), color));
        for (int i = 0; i < indexCount; i++)
        {
            m_indexBuffer.push_back(m_indexCount + QuadIndices[i]);
        }

        growContext_V2F_T2F_C4F(image, vertexCount, indexCount);
    }

    void CanvasComponent::DrawArcImage(TextureAsset* image, AZ::Vector4& rect, float x, float y, float radius, float angle1, float angle2, const AZ::Color& color)
    {
        if (angle1 == angle2) return;
        if (!image->IsReady())
        {
            return;
        }
        AZ::Vector2 size(image->GetInfo().width, image->GetInfo().height);

        if (rect.GetX() < 0.0f) rect.SetX(0.0f);
        if (rect.GetY() < 0.0f) rect.SetY(0.0f);
        if (rect.GetZ() < 0.0f) rect.SetZ(size.GetX());
        if (rect.GetW() < 0.0f) rect.SetW(size.GetY());

        rect.SetX(AZStd::clamp((float)rect.GetX(), 0.0f, size.GetX()));
        rect.SetY(AZStd::clamp((float)rect.GetY(), 0.0f, size.GetY()));
        rect.SetZ(AZStd::clamp((float)rect.GetZ(), 0.0f, size.GetX() - (float)rect.GetX()));
        rect.SetW(AZStd::clamp((float)rect.GetW(), 0.0f, size.GetY() - (float)rect.GetY()));

        const auto hw = rect.GetZ() / size.GetX() * 0.5f;
        const auto hh = rect.GetW() / size.GetY() * 0.5f;

        const AZ::Vector2 center((rect.GetX() + 0.5f * rect.GetZ()) / size.GetX(), (rect.GetY() + 0.5f * rect.GetW()) / size.GetY());

        int piceses = int(ceilf(calculatePiceses(radius) * fabsf(angle1 - angle2) / 360.0f));
        const float angle_shift = AZ::DegToRad((angle2 - angle1) / piceses);

        if (angle_shift == 0.0) return;
        const int vertexCount = piceses + 2;
        const int indexCount = piceses * 3;
        float phi = AZ::DegToRad(angle1 - 90);
        for (int i = 0; i <= piceses; ++i, phi += angle_shift)
        {
            m_vertexBuffer.push_back(VertexData(x + radius * cosf(phi), y + radius * sinf(phi), center.GetX() + cosf(phi) * hw, center.GetY() + sinf(phi) * hh, color));
        }
        for (int i = 0; i < piceses; ++i)
        {
            m_indexBuffer.push_back(m_indexCount);
            m_indexBuffer.push_back(m_indexCount + i + 1);
            m_indexBuffer.push_back(m_indexCount + i + 2);
        }

        growContext_V2F_T2F_C4F(image, vertexCount, indexCount);
    }
    
    void CanvasComponent::DrawCircleImage(TextureAsset* image, AZ::Vector4& rect, float x, float y, float radius, const AZ::Color& color)
    {
        DrawArcImage(image, rect, x, y, radius, 0.0f, 360.0f, color);
    }
    void CanvasComponent::DrawArc(float x, float y, float radius, float angle1, float angle2, const AZ::Color& color)
    {
        if (angle1 == angle2) return;

        int piceses = int(ceilf(calculatePiceses(radius) * fabsf(angle1 - angle2) / 360.0f));

        const int vertexCount = piceses + 2;
        const int indexCount = piceses * 3;

        const float angle_shift = AZ::DegToRad((angle2 - angle1) / piceses);

        if (angle_shift == 0.0) return;

        m_vertexBuffer.push_back(VertexData(x, y, 0, 0, color));

        float phi = AZ::DegToRad(angle1 - 90);
        for (int i = 0; i <= piceses; ++i, phi += angle_shift)
        {
            m_vertexBuffer.push_back(VertexData(x + radius * cosf(phi), y + radius * sinf(phi), 0, 0, color));
        }
        for (int i = 0; i < piceses; ++i)
        {
            m_indexBuffer.push_back(m_indexCount);
            m_indexBuffer.push_back(m_indexCount + i + 1);
            m_indexBuffer.push_back(m_indexCount + i + 2);
        }
        growContext_V2F_C4F(vertexCount, indexCount);
    }

    void CanvasComponent::DrawCircle(float x, float y, float radius, const AZ::Color& color)
    {
        DrawArc(x, y, radius, 0.0f, 360.0f, color);
    }

    void CanvasComponent::DrawRectangle(float x, float y, float w, float h, const AZ::Color& color)
    {
        static const auto vertexCount = 4;
        static const auto indexCount = 6;

        m_vertexBuffer.push_back(VertexData(x, y, 0, 0, color));
        m_vertexBuffer.push_back(VertexData(x + w, y, 0, 0, color));
        m_vertexBuffer.push_back(VertexData(x + w, y + h, 0, 0, color));
        m_vertexBuffer.push_back(VertexData(x + w, y + h, 0, 0, color));
        for (int i = 0; i < indexCount; i++)
        {
            m_indexBuffer.push_back(m_indexCount + QuadIndices[i]);
        }

        growContext_V2F_C4F(vertexCount, indexCount);
    }

    void CanvasComponent::DrawLine(float pointA_x, float pointA_y, float pointB_x, float pointB_y, float lineWidth, const AZ::Color& color)
    {
        static const auto vertexCount = 4;
        static const auto indexCount = 6;

        float length = sqrt((pointB_x - pointA_x) * (pointB_x - pointA_x) + (pointB_y - pointA_y) * (pointB_y - pointA_y));
        AZ::Vector2 UnitDir = AZ::Vector2(pointB_x - pointA_x, pointB_y - pointA_y) * (1 / length);
        AZ::Vector2 UnitDirAdd90 = AZ::Vector2(-UnitDir.GetY(), UnitDir.GetX());

        m_vertexBuffer.push_back(VertexData(pointA_x + UnitDirAdd90.GetX() * lineWidth / 2.0f, pointA_y + UnitDirAdd90.GetY() * lineWidth / 2.0f, 0, 0, color));
        m_vertexBuffer.push_back(VertexData(pointB_x + UnitDirAdd90.GetX() * lineWidth / 2.0f, pointB_y + UnitDirAdd90.GetY() * lineWidth / 2.0f, 0, 0, color));
        m_vertexBuffer.push_back(VertexData(pointB_x - UnitDirAdd90.GetX() * lineWidth / 2.0f, pointB_y - UnitDirAdd90.GetY() * lineWidth / 2.0f, 0, 0, color));
        m_vertexBuffer.push_back(VertexData(pointA_x - UnitDirAdd90.GetX() * lineWidth / 2.0f, pointA_y - UnitDirAdd90.GetY() * lineWidth / 2.0f, 0, 0, color));

        for (int i = 0; i < indexCount; i++)
        {
            m_indexBuffer.push_back(m_indexCount + QuadIndices[i]);
        }

        growContext_V2F_C4F(vertexCount, indexCount);
    }

    void CanvasComponent::Clear()
    {
        m_indexCount = 0;
        m_vertexCount = 0;
        m_currentContextIndex = -1;

        m_vertexBuffer.clear();
        m_indexBuffer.clear();
        m_contextQueue.clear();
    
    }
    /*
    void CanvasComponent::Render2D()
    {
        if (m_vertexCount > 0 && m_indexCount > 0)
        {
            bgfx::updateDynamicVertexBuffer(m_vertexBufferHandle, 0, bgfx::copy(m_vertexBuffer.data(), m_vertexBuffer.size() * sizeof(VertexData)));
            bgfx::updateDynamicIndexBuffer(m_indexBufferHandle, 0, bgfx::copy(m_indexBuffer.data(), m_indexBuffer.size() * sizeof(AZ::u16)));
        }

        int width, height;
        EBUS_EVENT(WindowSystemRequestBus, GetWindowSize, width, height);
        float proj[16];
        bx::mtxOrtho(proj, 0.0f, float(width), 0.0f, float(height), -100.0f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, nullptr, proj);
        // bgfx::setTransform(m_modelTM);
        bgfx::setViewMode(0, bgfx::ViewMode::Sequential);
        for (int i = 0; i < m_contextQueue.size(); i++)
        {
            if (m_contextQueue[i].Type == GfxMaterialType::V2F_C4F)
            {
                bgfx::setVertexBuffer(0, m_vertexBufferHandle);
                bgfx::setIndexBuffer(m_indexBufferHandle, m_contextQueue[i].Begin, m_contextQueue[i].Count);
                EBUS_EVENT(CanvasSystemRequestBus, SubmitWithGfx_Col);
            }
            else
            {
                if (m_contextQueue[i].Texture && m_contextQueue[i].Texture->IsReady())
                {
                    bgfx::setVertexBuffer(0, m_vertexBufferHandle);
                    bgfx::setIndexBuffer(m_indexBufferHandle, m_contextQueue[i].Begin, m_contextQueue[i].Count);
                    EBUS_EVENT(CanvasSystemRequestBus, SubmitWithGfx_Tex, m_contextQueue[i].Texture->GetHandle());
                }
            }
        }
    }
    */

    void CanvasComponent::UpdateMeshBuffer()
    {
        int width, height;
        EBUS_EVENT(WindowSystemRequestBus, GetWindowSize, width, height);

        float proj[16];
        bx::mtxOrtho(proj, 0.0f, float(width), 0.0f, float(height), -100.0f, 100.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, nullptr, proj);
        bgfx::setViewMode(0, bgfx::ViewMode::Sequential);

        if (!m_vertexBuffer.empty() && !m_indexBuffer.empty())
        {
            bgfx::updateDynamicVertexBuffer(m_vertexBufferHandle, 0, bgfx::copy(m_vertexBuffer.data(), m_vertexBuffer.size() * sizeof(VertexData)));
            bgfx::updateDynamicIndexBuffer(m_indexBufferHandle, 0, bgfx::copy(m_indexBuffer.data(), m_indexBuffer.size() * sizeof(AZ::u16)));
        }
    }
    bool CanvasComponent::ApplySubMesh(int index)
    {
        if (!IsValid()) return false;

        bgfx::setTransform(m_modelTM);
        bgfx::setVertexBuffer(0, m_vertexBufferHandle);
        auto& context = m_contextQueue.at(index);
        bgfx::setIndexBuffer(m_indexBufferHandle, context.Begin, context.Count);
        
        return true;
    }

    inline void CanvasComponent::growContext_V2F_T2F_C4F(TextureAsset* image, int vertexCount, int indexCount)
    {
        if (m_currentContextIndex < 0)
        {
            DrawContext dc;
            dc.Type = GfxMaterialType::V2F_T2F_C4F;
            dc.Texture = image;
            dc.Begin = m_indexCount;
            dc.Count = 0;
            m_contextQueue.push_back(dc);
            m_currentContextIndex = 0;
            AZStd::shared_ptr<Module::Material> material = GetMaterial(dc.Type, dc.Texture);
            if (material)
            {
                EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, SetMaterialAndMesh, material, m_currentContextIndex);
            }
        }
        else
        {
            if (m_contextQueue[m_currentContextIndex].Type != GfxMaterialType::V2F_T2F_C4F|| m_contextQueue[m_currentContextIndex].Texture!=image)
            {
                DrawContext dc;
                dc.Type = GfxMaterialType::V2F_T2F_C4F;
                dc.Texture = image;
                dc.Begin = m_indexCount;
                dc.Count = 0;
                m_contextQueue.push_back(dc);
                m_currentContextIndex++;
                AZStd::shared_ptr<Module::Material> material = GetMaterial(dc.Type, dc.Texture);
                if (material)
                {
                    EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, SetMaterialAndMesh, material, m_currentContextIndex);
                }
            }
        }
        m_contextQueue[m_currentContextIndex].Count += indexCount;
        m_vertexCount += vertexCount;
        m_indexCount += indexCount;
    }
    /*
    void CanvasComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
    {
        
        for (int i = 0; i <= m_currentContextIndex; i++)
        {
            if (m_contextQueue[i].Type == GfxMaterialType::V2F_T2F_C4F)
            {
                AZStd::shared_ptr<Module::Material> material = GetMaterial(m_contextQueue[i].Type, m_contextQueue[i].Texture);
                if (material)
                {
                    EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, SetMaterialAndMesh, material, i);
                }
            }
            else
            {
                AZStd::shared_ptr<Module::Material> material = GetMaterial(m_contextQueue[i].Type, m_contextQueue[i].Texture);
                if (material)
                {
                    EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, SetMaterialAndMesh, material, i);
                }
            }
        }
       
    }
    */
    inline void CanvasComponent::growContext_V2F_C4F(int vertexCount, int indexCount)
    {
        if (m_currentContextIndex < 0)
        {
            DrawContext dc;
            dc.Type = GfxMaterialType::V2F_C4F;
            dc.Begin = m_indexCount;
            dc.Count = 0;
            m_contextQueue.push_back(dc);
            m_currentContextIndex = 0;
            AZStd::shared_ptr<Module::Material> material = GetMaterial(dc.Type, dc.Texture);
            if (material)
            {
                EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, SetMaterialAndMesh, material, m_currentContextIndex);
            }
        }
        else
        {
            if (m_contextQueue[m_currentContextIndex].Type != GfxMaterialType::V2F_C4F)
            {
                DrawContext dc;
                dc.Type = GfxMaterialType::V2F_C4F;
                dc.Begin = m_indexCount;
                dc.Count = 0;
                m_contextQueue.push_back(dc);
                m_currentContextIndex++;
                AZStd::shared_ptr<Module::Material> material = GetMaterial(dc.Type, dc.Texture);
                if (material)
                {
                    EBUS_EVENT_ID(GetEntityId(), Module::RendererRequestBus, SetMaterialAndMesh, material, m_currentContextIndex);
                }
            }
        }
        m_contextQueue[m_currentContextIndex].Count += indexCount;
        m_vertexCount += vertexCount;
        m_indexCount += indexCount;
    }
}
