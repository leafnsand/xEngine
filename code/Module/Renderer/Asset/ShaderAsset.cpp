#include "Renderer/Asset/ShaderAsset.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace Module
{
    void ShaderAsset::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<Pass>()
                ->Field("name", &Pass::m_name)
                ->Field("vs", &Pass::m_vs)
                ->Field("fs", &Pass::m_fs)
                ->Field("rs", &Pass::m_rs)
                ;

            serializeContext->Class<ShaderAsset>()
                ->Field("name", &ShaderAsset::m_name)
                ->Field("passes", &ShaderAsset::m_passes)
                ;
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<Pass>("Pass")
                ->Constructor()
                ->Property("name", BehaviorValueProperty(&Pass::m_name))
                ->Property("vs", BehaviorValueProperty(&Pass::m_vs))
                ->Property("fs", BehaviorValueProperty(&Pass::m_fs))
                ->Property("rs", BehaviorValueProperty(&Pass::m_rs))
                ;

            behaviorContext->Class<ShaderAsset>("ShaderAsset")
                ->Constructor()
                ->Property("name", BehaviorValueProperty(&ShaderAsset::m_name))
                ->Property("passes", BehaviorValueProperty(&ShaderAsset::m_passes))
                ;
        }
    }
}

namespace AZ
{
    AZ_TYPE_INFO_SPECIALIZE(Module::ShaderAsset::Pass, "{C9520603-AFD6-4DA0-977C-DC93F844BB94}");
}
