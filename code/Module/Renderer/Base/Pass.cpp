#include "Renderer/Base/Pass.h"

namespace Module
{
    Pass::Pass(const ShaderAsset::Pass& config)
    {
        m_vs.Create(config.m_vs.c_str(), true);
        m_fs.Create(config.m_fs.c_str(), true);

        AZ::Data::AssetBus::MultiHandler::BusConnect(m_vs.GetId());
        AZ::Data::AssetBus::MultiHandler::BusConnect(m_fs.GetId());

        m_defaultRs = m_rs = config.m_rs;
    }

    Pass::Pass(Pass&& rhv) noexcept
    {
        operator=(AZStd::move(rhv));
    }

    Pass& Pass::operator=(Pass&& rhv) noexcept
    {
        AZ::Data::AssetBus::MultiHandler::operator=(AZStd::move(rhv));

        if (bgfx::isValid(m_program))
        {
            bgfx::destroy(m_program);
        }

        m_vs = rhv.m_vs;
        m_fs = rhv.m_fs;
        m_program = rhv.m_program;
        m_defaultRs = rhv.m_defaultRs;
        m_rs = rhv.m_rs;

        rhv.m_program = BGFX_INVALID_HANDLE;

        return *this;
    }

    Pass::~Pass()
    {
        if (bgfx::isValid(m_program))
        {
            bgfx::destroy(m_program);
        }
    }

    void Pass::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        AZ::Data::AssetBus::MultiHandler::BusDisconnect(asset.GetId());

        if (m_vs.IsReady() && m_fs.IsReady())
        {
            auto vs = bgfx::createShader(bgfx::makeRef(m_vs.Get()->GetBuffer(), m_vs.Get()->GetLength()));
            auto fs = bgfx::createShader(bgfx::makeRef(m_fs.Get()->GetBuffer(), m_fs.Get()->GetLength()));

            m_program = bgfx::createProgram(vs, fs, true);

            m_vs.Release();
            m_fs.Release();
        }
    }

    void Pass::SetReverseCull(bool value)
    {
        m_rs = m_defaultRs;

        if (value)
        {
            const auto state = m_rs & BGFX_STATE_CULL_MASK;

            m_rs &= ~BGFX_STATE_CULL_MASK;

            if (state == BGFX_STATE_CULL_CW)
            {
                m_rs |= BGFX_STATE_CULL_CCW;
            }
            else if (state == BGFX_STATE_CULL_CCW)
            {
                m_rs |= BGFX_STATE_CULL_CW;
            }
        }
    }

    void Pass::Apply(bgfx::ViewId viewId) const
    {
        if (bgfx::isValid(m_program))
        {
            bgfx::setState(m_rs);
            bgfx::submit(viewId, m_program);
        }
    }
}
