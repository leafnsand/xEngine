#pragma once

#include "Renderer/Base/Texture.h"

#include <AzCore/std/smart_ptr/shared_ptr.h>

namespace Module
{
    class Sprite
    {
    public:
        AZ_CLASS_ALLOCATOR(Sprite, AZ::SystemAllocator, 0);
        AZ_RTTI(Sprite, "{CD26E3BA-AADB-406A-9F7C-B77E5AC23273}");

        Sprite();
        Sprite(const AZStd::string& texturePath, const AZStd::string& spriteName);

        // non-copyable
        Sprite(const Sprite&) = delete;
        Sprite& operator=(const Sprite&) = delete;

        static void Reflect(AZ::ReflectContext* context);

        bool IsValid();

    private:
        TexturePtr                m_texture;
        AZStd::string             m_spriteName;
        TextureAsset::SpriteData* m_spriteData = nullptr;
    };

    using SpritePtr = AZStd::shared_ptr<Sprite>;
}