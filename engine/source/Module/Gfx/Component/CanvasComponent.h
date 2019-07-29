#pragma once

#include "AzCore/Component/Component.h"
#include "AzCore/Component/TransformBus.h"
#include "Gfx/EBus/CanvasComponentBus.h"
#include "AzCore/Component/TickBus.h"
#include "Renderer/EBus/RendererSystemComponentBus.h"

#include "bgfx/bgfx.h"

#include "Renderer/EBus/MeshProviderBus.h"


namespace Module
{
    enum GfxMaterialType
    {
        V2F_T2F_C4F,
        V2F_C4F,
    };

    class CanvasComponent
        :public AZ::Component
        , protected AZ::TransformNotificationBus::Handler
        , protected CanvasRequestBus::Handler
        // , protected RendererSystemNotificationBus::Handler
        // , protected AZ::TickBus::Handler
        , protected MeshProviderRequestBus::Handler
    {
    public:
        AZ_COMPONENT(CanvasComponent, "{9BE4EE65-01D6-4E16-A782-CF568FFADA57}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("CanvasService"));
            provided.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("CanvasService"));
            incompatible.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            dependent.push_back(AZ_CRC("TransformService"));
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC("TransformService"));
        }
        static void Reflect(AZ::ReflectContext* reflection);

        CanvasComponent(){};

        // AZ::Component
        void Activate() override;
        void Deactivate() override;
    protected:
        // AZ::TickBus::Handler
        // void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        // AZ::TransformBus::Handler
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world);
        // CanvasRequestBus::Handler
         bool IsValid() const override;
         void DrawImage(TextureAsset* image, AZ::Vector4& rect, AZ::Matrix4x4& transform, const AZ::Color& color) override;
         void DrawArcImage(TextureAsset* image, AZ::Vector4& rect, float x, float y, float radius, float angle1, float angle2, const AZ::Color& color) override;
         void DrawCircleImage(TextureAsset* image, AZ::Vector4& rect, float x, float y, float radius, const AZ::Color& color) override;
         void DrawArc(float x, float y, float radius, float angle1, float angle2, const AZ::Color& color) override;
         void DrawCircle(float x, float y, float radius, const AZ::Color& color) override;
         void DrawRectangle(float x, float y, float w, float h, const AZ::Color& color) override;
         void DrawLine(float pointA_x, float pointA_y, float pointB_x, float pointB_y, float lineWidth, const AZ::Color& color) override;
         void Clear() override;
         //----RendererSystemNotificationBus::Handler
         //----void Render2D() override;
         // MeshProviderRequestBus::Handler
         void UpdateMeshBuffer() override;
         bool ApplySubMesh(int index) override;
    private:
        AZStd::string m_name;

        struct VertexData
        {
            VertexData() {}
            VertexData(float px, float py, float pu, float pv,const AZ::Color c) :
                x(px), y(py),
                u(pu), v(pv),
                r(c.GetR()), g(c.GetG()), b(c.GetB()), a(c.GetA()) {}

            float x, y, u, v;
            float r, g, b, a;
        };

        bgfx::DynamicVertexBufferHandle m_vertexBufferHandle = BGFX_INVALID_HANDLE;
        bgfx::DynamicIndexBufferHandle m_indexBufferHandle = BGFX_INVALID_HANDLE;

        int m_vertexCount = 0, m_indexCount = 0;
        AZStd::vector<VertexData> m_vertexBuffer;
        AZStd::vector<uint16_t> m_indexBuffer;

        struct DrawContext
        {
            GfxMaterialType Type = GfxMaterialType::V2F_C4F;
            TextureAsset* Texture = nullptr;
            int Begin = 0, Count = 0;
        };

        int m_currentContextIndex = -1;
        AZStd::vector<DrawContext> m_contextQueue;

        float m_modelTM[16];

        inline void growContext_V2F_T2F_C4F(TextureAsset* image, int vertexCount, int indexCount);
        inline void growContext_V2F_C4F(int vertexCount, int indexCount);
    };
}
