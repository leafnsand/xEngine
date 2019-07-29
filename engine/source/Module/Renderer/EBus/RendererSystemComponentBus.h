#pragma once

#include "Renderer/Base/Shader.h"
#include "Renderer/Base/Material.h"
#include "Renderer/Base/Mesh.h"
#include "Renderer/Base/Texture.h"
#include "Renderer/Base/Sprite.h"

#include <AzCore/EBus/EBus.h>

namespace Module
{
    enum class BasicMeshType : AZ::u8
    {
        Cube,
        Sphere,
        Capsule,
        Cylinder,
        Plane,
    };

    class RendererSystemRequest : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        ~RendererSystemRequest() = default;

        virtual void SetResetFlags(uint32_t resetFlags) = 0;

        virtual TexturePtr  GetTexture(const AZStd::string& path) = 0;

        virtual ShaderPtr   GetShader(const AZStd::string& path) = 0;

        virtual MaterialPtr GetMaterial(const AZStd::string& path) = 0;

        virtual MeshPtr     GetMesh(AZStd::string& path) = 0;
        
        virtual SpritePtr   GetSprite(const AZStd::string& path, const AZStd::string& spriteName) = 0;
    };

    using RendererSystemRequestBus = AZ::EBus<RendererSystemRequest>;
}
