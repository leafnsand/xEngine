#include "FairyGUI/fairy/display/Container.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Component/GameEntityContextBus.h>
#include <AzCore/Component/TransformComponent.h>

NS_FGUI_BEGIN

Container::Container(const AZStd::string& entityName, GObject* target)
{
    this->m_owner = target;
    this->CreateEntity(entityName);
}

Container::Container(AZ::EntityId& entityId, GObject* target)
{
    this->m_owner = target;
    DisplayObject::SetEntity(entityId);
}

Container::~Container()
{
        
}

void Container::AddChild(DisplayObject* displayObject)
{
    if (!displayObject)
    {
        return;
    }
    displayObject->SetParent(this);
}

void Container::RemoveChild(DisplayObject* displayObject)
{
    displayObject->RemoveFromParent();
}


NS_FGUI_END