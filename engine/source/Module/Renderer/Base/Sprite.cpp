#include "Renderer/Base/Sprite.h"

#include "Renderer/EBus/RendererSystemComponentBus.h"

#include <AzCore/RTTI/BehaviorContext.h>

namespace Module
{
    Sprite::Sprite()
    {
    }

    Sprite::Sprite(const AZStd::string& texturePath, const AZStd::string& spriteName) : m_spriteName(spriteName)
    {
        EBUS_EVENT_RESULT(m_texture, RendererSystemRequestBus, GetTexture, texturePath);
    }

    void Sprite::Reflect(AZ::ReflectContext* context)
    {
        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<Sprite>()
                ;
        }
    }

    bool Sprite::IsValid()
    {
        if (m_spriteData)
        {
            return true;
        }
        if (m_texture->IsValid())
        {
            auto* textureAsset = m_texture->m_asset.Get();
            if (textureAsset)
            {
                for (auto& spriteData : textureAsset->m_sprites)
                {
                    if (spriteData.m_name == m_spriteName)
                    {
                        m_spriteData = &spriteData;
                        return true;
                    }
                }
            }
        }
        return false;
    }
}
