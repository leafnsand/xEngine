#include "Cursor.h"
#include "GComponent.h"

NS_FGUI_BEGIN

void Cursor::SetOffset(Vec2 offset)
{
    _offset = offset;
}

void Cursor::SetParentNode(GObject* obj)
{
    _parentNode = obj;
}

void Cursor::DrawCursor()
{
//    this->drawRect(1, 30, 0, Color4F::BLACK, Color4F::BLACK);
}

AZ::Transform Cursor::GetGlobalMatrix44()
{
    AZ::Transform result = GetLocalMatrix44();
    if(_parentNode)
    {
//        result.translate(AZ::Vector4(_offset.x, _offset.y, 0, 0));
//        result = AZ::Transform::multiply(result, _parentNode->GetLocalMatrix44());
//        GComponent* pParent = _parentNode->getParent();
//        while (pParent)
//        {
//            result = AZ::Transform::multiply(result, pParent->GetLocalMatrix44());
//            pParent = pParent->getParent();
//        }
    }
    return result;
}
NS_FGUI_END