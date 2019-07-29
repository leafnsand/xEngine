#include "Renderer/Base/Shader.h"

#include <AzCore/StringFunc/StringFunc.h>

namespace Module
{
    Shader::Shader(const AZStd::string& relativePath)
    {        
        AZStd::string shaderName;
        AZ::StringFunc::Path::Join((relativePath + ".shader").c_str(), "config.shader", shaderName);

        m_config.Create(shaderName.c_str(), true);

        AZ::Data::AssetBus::Handler::BusConnect(m_config.GetId());
    }

    void Shader::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        AZ::Data::AssetBus::Handler::BusDisconnect(asset.GetId());

        for (auto& passConfig : m_config.Get()->m_passes)
        {
            m_passes.emplace_back(passConfig);
        }

        m_config.Release();
    }

    void Shader::SetReverseCull(bool flag)
    {
        for (auto& pass : m_passes)
        {
            pass.SetReverseCull(flag);
        }
    }

    bool Shader::IsValid() const
    {
        return !m_passes.empty() && m_passes.end() == AZStd::find_if(m_passes.begin(), m_passes.end(), [](const Pass& pass)
        {
            return !pass.IsValid();
        });
    }
}
