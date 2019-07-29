#include "Renderer/Base/Material.h"

#include "Renderer/EBus/RendererSystemComponentBus.h"

#include <AzCore/RTTI/BehaviorContext.h>

namespace Module
{
    Material::Property::Property(const MaterialAsset::Property& config)
    {
        m_name = config.m_name;
        if (config.m_type == MaterialAsset::Property::Type::Vector)
        {
            m_uniform = bgfx::createUniform(config.m_name.c_str(), bgfx::UniformType::Vec4);
            m_type = Type::Vector;
            config.m_vectorValue.StoreToFloat4(m_vectorValue);
        }
        else if (config.m_type == MaterialAsset::Property::Type::Texture2D ||
            config.m_type == MaterialAsset::Property::Type::Texture3D ||
            config.m_type == MaterialAsset::Property::Type::TextureCube)
        {
            m_uniform = bgfx::createUniform(config.m_name.c_str(), bgfx::UniformType::Int1);
            m_type = Type::Texture;
            EBUS_EVENT_RESULT(m_textureValue, RendererSystemRequestBus, GetTexture, config.m_textureValue);
        }
    }

    Material::Property::~Property()
    {
        if (bgfx::isValid(m_uniform))
        {
            bgfx::destroy(m_uniform);
        }
    }

    void Material::Reflect(AZ::ReflectContext* context)
    {
        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<Material>()
                ;
        }
    }

    Material::Property::Property(Property&& rhv) noexcept
    {
        operator=(AZStd::move(rhv));
    }

    Material::Property& Material::Property::operator=(Property&& rhv) noexcept
    {
        m_uniform = rhv.m_uniform;
        m_name = rhv.m_name;
        m_type = rhv.m_type;
        memcpy(m_vectorValue, rhv.m_vectorValue, sizeof(m_vectorValue));
        m_textureValue = rhv.m_textureValue;

        rhv.m_uniform = BGFX_INVALID_HANDLE;

        return *this;
    }

    void Material::Property::Apply(int textureStage)
    {
        if (m_type == Type::Vector)
        {
            bgfx::setUniform(m_uniform, m_vectorValue);
        }
        else if (m_type == Type::Texture)
        {
            m_textureValue->Apply(textureStage, m_uniform);
        }
    }

    Material::Material(const AZStd::string& relativePath)
    {
        m_config.Create((relativePath + ".mat").c_str(), true);

        AZ::Data::AssetBus::Handler::BusConnect(m_config.GetId());
    }

    void Material::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        AZ::Data::AssetBus::Handler::BusDisconnect(asset.GetId());

        EBUS_EVENT_RESULT(m_shader, RendererSystemRequestBus, GetShader, m_config.Get()->m_shader);

        m_queue = m_queue == 0 ? m_config.Get()->m_queue : m_queue;

        for (auto& propertyConfig : m_config.Get()->m_properties)
        {
            m_properties.emplace_back(propertyConfig);
        }

        m_config.Release();
    }

    void Material::SetVector(const AZStd::string& name, const AZ::Vector4& vector)
    {
        for (auto& property : m_properties)
        {
            if (property.m_type == Property::Type::Vector && property.m_name == name)
            {
                vector.StoreToFloat4(property.m_vectorValue);

                return;
            }
        }
    }

    void Material::SetTexture(const AZStd::string& name, TexturePtr texture)
    {
        for (auto& property : m_properties)
        {
            if (property.m_type == Property::Type::Texture && property.m_name == name)
            {
                property.m_textureValue = texture;
                return;
            }
        }
    }

    bool Material::IsValid() const
    {
        return m_shader && m_shader->IsValid();
    }

    void Material::SetReverseCull(bool value)
    {
        // TODO cache value if shader is not valid
        if (IsValid())
        {
            m_shader->SetReverseCull(value);
        }
    }

    void Material::Apply()
    {
        if (IsValid())
        {
            int textureStage = 0;
            for (auto& property : m_properties)
            {
                property.Apply(textureStage);
                if (property.m_type == Property::Type::Texture)
                {
                    ++textureStage;
                }
            }
        }
    }

    void Material::SetQueue(AZ::s32 queue)
    {
        m_queue = queue;
    }
}
