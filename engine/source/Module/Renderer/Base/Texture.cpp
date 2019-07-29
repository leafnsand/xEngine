#include "Renderer/Base/Texture.h"

#include <AzCore/RTTI/BehaviorContext.h>

namespace Module
{
    Texture::Texture()
    {
    }

    Texture::Texture(const AZStd::string& path)
    {
        m_asset.Create(path.c_str(), true);
        AZ::Data::AssetBus::Handler::BusConnect(m_asset.GetId());
    }

    Texture::Texture(Texture&& rhv) noexcept
    {
        operator=(AZStd::move(rhv));
    }

    Texture& Texture::operator=(Texture&& rhv) noexcept
    {
        AZ::Data::AssetBus::Handler::operator=(AZStd::move(rhv));

        m_asset = rhv.m_asset;
        m_handle = rhv.m_handle;
        m_info = rhv.m_info;

        rhv.m_handle = BGFX_INVALID_HANDLE;

        return *this;
    }

    Texture::~Texture()
    {
        if (bgfx::isValid(m_handle))
        {
            bgfx::destroy(m_handle);
        }
    }

    void Texture::Reflect(AZ::ReflectContext* context)
    {
        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<Texture>()
                ;
        }
    }

    void Texture::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        AZ::Data::AssetBus::Handler::BusDisconnect();

        m_handle = m_asset.Get()->m_handle;
        m_info = m_asset.Get()->m_info;
    }

    void Texture::Apply(int stage, bgfx::UniformHandle uniform)
    {
        if (bgfx::isValid(uniform) && bgfx::isValid(m_handle))
        {
            bgfx::setTexture(stage, uniform, m_handle);
        }
    }
}
