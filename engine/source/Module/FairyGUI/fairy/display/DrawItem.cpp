#include "FairyGUI/fairy/display/DrawItem.h"

NS_FGUI_BEGIN

DrawItem::DrawItem(const AZStd::string& entityName,GObject* target)
{
    this->m_owner = target;
    this->CreateEntity(entityName);
    
}

DrawItem::~DrawItem()
{
    
}



NS_FGUI_END