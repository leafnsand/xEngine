#include "Renderer/Asset/MaterialAsset.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace Module
{
    void MaterialAsset::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<Property>()
                ->Field("name", &Property::m_name)
                ->Field("type", &Property::m_type)
                ->Field("vectorValue", &Property::m_vectorValue)
                ->Field("textureValue", &Property::m_textureValue)
                ;

            serializeContext->Class<MaterialAsset>()
                ->Field("name", &MaterialAsset::m_name)
                ->Field("shader", &MaterialAsset::m_shader)
                ->Field("queue", &MaterialAsset::m_queue)
                ->Field("properties", &MaterialAsset::m_properties)
                ;
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<Property>("Property")
                ->Constructor()
                ->Property("name", BehaviorValueProperty(&Property::m_name))
                ->Property("type",
                    [](Property* component) { return reinterpret_cast<int&>(component->m_type); },
                    [](Property* component, const int& value) { component->m_type = static_cast<Property::Type>(value); })
                ->Property("vectorValue", BehaviorValueProperty(&Property::m_vectorValue))
                ->Property("textureValue", BehaviorValueProperty(&Property::m_textureValue))
                ;

            behaviorContext->Class<MaterialAsset>("MaterialAsset")
                ->Constructor()
                ->Property("name", BehaviorValueProperty(&MaterialAsset::m_name))
                ->Property("shader", BehaviorValueProperty(&MaterialAsset::m_shader))
                ->Property("queue", BehaviorValueProperty(&MaterialAsset::m_queue))
                ->Property("properties", BehaviorValueProperty(&MaterialAsset::m_properties))
                ;
        }
    }
}

namespace AZ
{
    AZ_TYPE_INFO_SPECIALIZE(Module::MaterialAsset::Property, "{35E1151E-9F7C-4390-AAF8-59ECDAAABD24}");
}
