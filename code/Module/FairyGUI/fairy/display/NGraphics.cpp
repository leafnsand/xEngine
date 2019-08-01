#include "FairyGUI/fairy/display/NGraphics.h"
#include "FairyGUI/fairy/display/DisplayObject.h"



#include <AzCore/Component/GameEntityContextBus.h>
#include "FairyGUI/Component/FairyMeshComponent.h"
#include "Renderer/Component/RendererComponent.h"

NS_FGUI_BEGIN

NGraphics::NGraphics(DisplayObject* displayObj)
{
    m_target = displayObj;
    AZ::Entity* targetEntity = nullptr;
    EBUS_EVENT_RESULT(targetEntity, AZ::GameEntityContextRequestBus, FindGameEntityById, m_target->m_entityId);
    if (targetEntity)
    {
        EBUS_EVENT(AZ::GameEntityContextRequestBus, DeactivateGameEntity, m_target->m_entityId);
        targetEntity->CreateComponent(Module::RendererComponent::CreateDescriptor()->GetUuid());
        targetEntity->CreateComponent(Module::FairyMeshComponent::CreateDescriptor()->GetUuid());
        EBUS_EVENT(AZ::GameEntityContextRequestBus, ActivateGameEntity, m_target->m_entityId);
    }
}

NGraphics::~NGraphics()
{
    
}

void NGraphics::RefreshData()
{
    EBUS_EVENT_ID(m_target->m_entityId, Module::FairyMeshRequestBus, RefreshData, this);
}


NS_FGUI_END