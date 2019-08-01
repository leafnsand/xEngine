#pragma once

#include <AzCore/Component/Component.h>

#include "Renderer/EBus/SpriteRendererComponentBus.h"
#include "Renderer/Component/RendererComponent.h"
#include "Renderer/Base/Sprite.h"

namespace Module
{
    class SpriteRendererComponent
        : public RendererComponent
        , protected SpriteRendererRequestBus::Handler
    {
    public:
        AZ_COMPONENT(SpriteRendererComponent, "{0C3E1D18-DBBF-41B8-AFAB-E28F89650BD6}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("MeshProviderService"));
            provided.push_back(AZ_CRC("RendererService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("RendererService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
        }

        static void Reflect(AZ::ReflectContext* context);

        /////////////////////////////////////////////////////////////////////////////////////
        // AZ::Component
        void Activate() override;
        void Deactivate() override;
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        // SpriteRendererRequestBus::Handler
        SpritePtr        GetSprite() const                override { return m_sprite; }
        void             SetSprite(SpritePtr sprite)      override { m_sprite = sprite; }

        const AZ::Color& GetColor() const                 override { return m_color; }
        void             SetColor(const AZ::Color& value) override { m_color = value; }

        SpriteDrawMode   GetDrawMode() const              override { return m_drawMode; }
        void             SetDrawMode(SpriteDrawMode mode) override { m_drawMode = mode; }

        bool             IsFilpX() const                  override { return m_isFlipX; }
        void             SetFlipX(bool value)             override { m_isFlipX = value; }

        bool             IsFlipY() const                  override { return m_isFlipY; }
        void             SetFlipY(bool value)             override { m_isFlipY = value; }
        /////////////////////////////////////////////////////////////////////////////////////

        void Render(size_t subMeshIndex) override;

    private:
        SpritePtr      m_sprite;
        AZ::Color      m_color;
        SpriteDrawMode m_drawMode = SpriteDrawMode::Simple;
        bool           m_isFlipX  = false;
        bool           m_isFlipY  = false;
    };
}
