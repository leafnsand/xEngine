#pragma once

#include "FairyGUI/fairy/FairyGUIMacros.h"
#include "FairyGUI/fairy/display/NGraphics.h"
#include "FairyGUI/fairy/render/RenderTypes.h"

#include <AzCore/Component/Entity.h>
#include <AzCore/std/string/string.h>
NS_FGUI_BEGIN

class GObject;
class Container;


class DisplayObject
{
public:
    DisplayObject();
    virtual ~DisplayObject();

    void CreateEntity(const AZStd::string& entityName);
    void SetEntity(AZ::EntityId& entityId);
    void DestroyEntity();
    void RemoveFromParent();
    void SetParent(Container* container);
    void RefreshNgraphicsData(DisplayObjRenderContext& context);
    void CreateGraphic();
    void ChangeVisibleState(bool value);

    void retain() {}
    void release() {}

    // 父级
    Container* m_parent = nullptr;
    // Entity对象ID
    AZ::EntityId m_entityId;
    // Owner 对象指针
    GObject* m_owner = nullptr;
    // 渲染节点
    NGraphics* m_graphics = nullptr;
    // 渲染配置
    DisplayObjRenderContext m_context;
};

NS_FGUI_END