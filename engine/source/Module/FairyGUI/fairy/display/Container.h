#pragma once

#include "FairyGUI/fairy/FairyGUIMacros.h"
#include "FairyGUI/fairy/display/DisplayObject.h"
#include "FairyGUI/fairy/base/Base.h"

#include <AzCore/std/string/string.h>
//#include <AzCore/std/containers/vector.h>

NS_FGUI_BEGIN

class Container : public DisplayObject
{
public:
    Container(const AZStd::string& entityName,GObject* target);
    Container(AZ::EntityId& entityId, GObject* target);
    Container(){}
    ~Container();

    void AddChild(DisplayObject* displayObject);

    void RemoveChild(DisplayObject* displayObject);

private:

};

NS_FGUI_END