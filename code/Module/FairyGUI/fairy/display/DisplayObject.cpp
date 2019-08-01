#include "FairyGUI/fairy/display/DisplayObject.h"
#include "FairyGUI/fairy/display/Container.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Component/GameEntityContextBus.h>
#include <AzCore/Component/TransformComponent.h>

NS_FGUI_BEGIN

DisplayObject::DisplayObject()
{
    
}

DisplayObject::~DisplayObject()
{
    
}

 void DisplayObject::CreateEntity(const AZStd::string& entityName)
 {
     AZ::Entity* targetEntity = nullptr;
     EBUS_EVENT_RESULT(targetEntity, AZ::GameEntityContextRequestBus, CreateGameEntity, entityName.c_str());
     if (targetEntity)
     {
         targetEntity->CreateComponent(AZ::TransformComponent::CreateDescriptor()->GetUuid());
         EBUS_EVENT(AZ::GameEntityContextRequestBus, ActivateGameEntity, targetEntity->GetId());
         EBUS_EVENT_ID(targetEntity->GetId(), AZ::TransformBus, SetLocalTranslation, AZ::Vector3(0, 0, 0));
     }
     this->m_entityId = targetEntity->GetId();
 }

void DisplayObject::SetEntity(AZ::EntityId& entityId)
{
    this->m_entityId = entityId;
}

void DisplayObject::DestroyEntity()
{
    if (this->m_entityId.IsValid())
    {
        EBUS_EVENT(AZ::GameEntityContextRequestBus, DestroyGameEntity, this->m_entityId);
    }
    this->m_entityId = AZ::EntityId();
}

void DisplayObject::RefreshNgraphicsData(DisplayObjRenderContext& context)
{
    this->m_context = context;
    if (!m_graphics)
    {
        m_graphics = new NGraphics(this);
    }
    m_graphics->RefreshData();
}

void DisplayObject::RemoveFromParent()
{
    EBUS_EVENT_ID(this->m_entityId, AZ::TransformBus, SetParent, AZ::EntityId());
}

void DisplayObject::SetParent(Container* container)
{
    EBUS_EVENT_ID(this->m_entityId, AZ::TransformBus, SetParent, container->m_entityId);
    EBUS_EVENT_ID(this->m_entityId, AZ::TransformBus, SetLocalTranslation, AZ::Vector3(0, 0, 0));
}

void DisplayObject::ChangeVisibleState(bool value)
{
    AZ::Entity* targetEntity = nullptr;
    EBUS_EVENT_RESULT(targetEntity, AZ::GameEntityContextRequestBus, FindGameEntityById, m_entityId);
    if (!targetEntity)
    {
        return;
    }
    if(value)
    {
        EBUS_EVENT(AZ::GameEntityContextRequestBus, ActivateGameEntity, targetEntity->GetId());
    }
    else
    {
        EBUS_EVENT(AZ::GameEntityContextRequestBus, DeactivateGameEntity, targetEntity->GetId());
    }
}


NS_FGUI_END