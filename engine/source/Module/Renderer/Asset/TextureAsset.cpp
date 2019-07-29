#include "Renderer/Asset/TextureAsset.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace Module
{
    void TextureAsset::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<TextureAsset>()
                ->Field("sprites", &TextureAsset::m_sprites)
                ->Field("pixelsToUnits", &TextureAsset::m_pixelsToUnits)
                ->Field("uWrapMode", &TextureAsset::m_uWrapMode)
                ->Field("vWrapMode", &TextureAsset::m_vWrapMode)
                ->Field("filterMode", &TextureAsset::m_filterMode)
                ->Field("meshType", &TextureAsset::m_meshType)
                ;

            serializeContext->Class<SpriteData>()
                ->Field("name", &SpriteData::m_name)
                ->Field("vertices", &SpriteData::m_positions)
                ->Field("texcoords", &SpriteData::m_texcoords)
                ->Field("indices", &SpriteData::m_indices)
                ->Field("border", &SpriteData::m_border)
                ->Field("size", &SpriteData::m_size)
                ->Field("pivot", &SpriteData::m_pivot)
                ->Field("pivotType", &SpriteData::m_pivotType)
                ;
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<TextureAsset>("TextureAsset")
                ->Constructor()
                ->Property("sprites", BehaviorValueProperty(&TextureAsset::m_sprites))
                ->Property("pixelsToUnits", BehaviorValueProperty(&TextureAsset::m_pixelsToUnits))
                ->Property("uWrapMode",
                    [](TextureAsset* component) { return reinterpret_cast<int&>(component->m_uWrapMode); },
                    [](TextureAsset* component, const int& value) { component->m_uWrapMode = static_cast<TextureWrapMode>(value); })
                ->Property("vWrapMode",
                    [](TextureAsset* component) { return reinterpret_cast<int&>(component->m_vWrapMode); },
                    [](TextureAsset* component, const int& value) { component->m_vWrapMode = static_cast<TextureWrapMode>(value); })
                ->Property("filterMode",
                    [](TextureAsset* component) { return reinterpret_cast<int&>(component->m_filterMode); },
                    [](TextureAsset* component, const int& value) { component->m_filterMode = static_cast<TextureFilterMode>(value); })
                ->Property("meshType",
                    [](TextureAsset* component) { return reinterpret_cast<int&>(component->m_meshType); },
                    [](TextureAsset* component, const int& value) { component->m_meshType = static_cast<SpriteMeshType>(value); })
                ;

            behaviorContext->Class<SpriteData>("SpriteData")
                ->Constructor()
                ->Property("name", BehaviorValueProperty(&SpriteData::m_name))
                ->Property("vertices", BehaviorValueProperty(&SpriteData::m_positions))
                ->Property("texcoords", BehaviorValueProperty(&SpriteData::m_texcoords))
                ->Property("indices", BehaviorValueProperty(&SpriteData::m_indices))
                ->Property("border", BehaviorValueProperty(&SpriteData::m_border))
                ->Property("size", BehaviorValueProperty(&SpriteData::m_size))
                ->Property("pivot", BehaviorValueProperty(&SpriteData::m_pivot))
                ->Property("pivotType",
                    [](SpriteData* component) { return reinterpret_cast<int&>(component->m_pivotType); },
                    [](SpriteData* component, const int& value) { component->m_pivotType = static_cast<SpritePivot>(value); })
                ;
        }
    }
}

namespace AZ
{
    AZ_TYPE_INFO_SPECIALIZE(Module::TextureAsset::SpriteData, "{844A6CB0-3970-4EC8-BCA3-242E154018AB}");
}
