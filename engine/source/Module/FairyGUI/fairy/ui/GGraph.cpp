#include "GGraph.h"
#include "utils/ToolSet.h"
//#include "n3gfx/gfxfacade.h"
#include "render/RenderDataProcess.h"
#include "display/DrawItem.h"

NS_FGUI_BEGIN


GGraph::GGraph() :
    _type(0),
    _lineSize(1),
    _lineColor(Color4F::BLACK),
    _fillColor(Color4F::WHITE)
//    _canvasId(GFX::InvalidId)
{
    _touchDisabled = true;
    this->m_displayObject = new DrawItem("GGraph", this);
}

GGraph::~GGraph()
{
//    if (_canvasId != GFX::InvalidId)
//    {
//        GFX::GFXFacade::Instance()->DestroyCanvas(_canvasId);
//    }
}

void GGraph::handleInit()
{
}

void GGraph::drawRect(float aWidth, float aHeight, int lineSize, const Color4F& lineColor, const Color4F& fillColor)
{
    _type = 0; //avoid updateshape call in handleSizeChange
    setSize(aWidth, aHeight);
    _type = 1;
    _lineSize = lineSize;
    _lineColor = lineColor;
    _fillColor = fillColor;

    _isDirty = true;
}

void GGraph::drawEllipse(float aWidth, float aHeight, int lineSize, const Color4F& lineColor, const Color4F& fillColor)
{
    _type = 0; //avoid updateshape call in handleSizeChange
    setSize(aWidth, aHeight);
    _type = 2;
    _lineSize = lineSize;
    _lineColor = lineColor;
    _fillColor = fillColor;

    _isDirty = true;
}

void GGraph::updateShape()
{
    if (_type == 1)
    {
        if (_lineSize > 0)
        {
//            GFX::GFXFacade::Instance()->DrawRectangle(_canvasId, -_lineSize, -_lineSize, _size.width + _lineSize * 2, _size.height + _lineSize * 2, AZ::Vector4(_lineColor.r, _lineColor.g, _lineColor.b, _lineColor.a));
        }
//        GFX::GFXFacade::Instance()->DrawRectangle(_canvasId, 0, 0, _size.width, _size.height, AZ::Vector4(_fillColor.r, _fillColor.g, _fillColor.b, _fillColor.a));
        _touchDisabled = false;
    }
    else if (_type == 2)
    {
//        float radiu = Math::n_max(_size.width / 2, _size.height / 2);
        if (_lineSize > 0)
        {
//            GFX::GFXFacade::Instance()->DrawCircle(_canvasId, 0, 0, radiu + _lineSize, AZ::Vector4(_lineColor.r, _lineColor.g, _lineColor.b, _lineColor.a));
        }
//        GFX::GFXFacade::Instance()->DrawCircle(_canvasId, 0, 0, radiu, AZ::Vector4(_fillColor.r, _fillColor.g, _fillColor.b, _fillColor.a));
        _touchDisabled = false;
    }
    else
        _touchDisabled = true;
}

Color4B GGraph::cg_getColor() const
{
    return (Color4B)_fillColor;
}

void GGraph::cg_setColor(const Color4B& value)
{
    _fillColor = (Color4F)value;
}

void GGraph::handleSizeChanged()
{
    GObject::handleSizeChanged();

    _isDirty = true;
}

void GGraph::handlePositionChanged()
{
    GObject::handlePositionChanged();

    _isDirty = true;
}

void GGraph::handleScaleChanged()
{
    GObject::handleScaleChanged();

    _isDirty = true;
}

void GGraph::setup_BeforeAdd(TXMLElement * xml)
{
    GObject::setup_BeforeAdd(xml);

    const char* p = xml->Attribute("type");
    if (p)
    {
        if (strcmp(p, "rect") == 0)
            _type = 1;
        else if (strcmp(p, "eclipse") == 0)
            _type = 2;
    }

    if (_type != 0)
    {
        p = xml->Attribute("lineSize");
        if (p)
            _lineSize = atoi(p);

        p = xml->Attribute("lineColor");
        if (p)
            _lineColor = (Color4F)ToolSet::convertFromHtmlColor(p);

        p = xml->Attribute("fillColor");
        if (p)
            _fillColor = (Color4F)ToolSet::convertFromHtmlColor(p);
    }

    _isDirty = true;
}

AZ::Transform GGraph::GetLocalMatrix44()
{
    if (_type == 2)
    {
//        _transform.setscalepivot2d(AZ::Vector2((_pivot.x - 0.5f) * _size.width, (_pivot.y - 0.5f) * _size.height));
//        _transform.setrotatepivot2d(AZ::Vector2((_pivot.x - 0.5f) * _size.width, (_pivot.y - 0.5f) * _size.height));
//
//        AZ::Transform matrix = _transform.getmatrix();
//
//        AZ::Vector4 scalef4 = AZ::Vector4(1, 1, 0, 1);
//        scalef4 = AZ::Vector4(1, 1, 0, 1);
//        float radiu = Math::n_max(_size.width / 2, _size.height / 2);
//        scalef4.X = scalef4.X * _size.width / 2 / radiu;
//        scalef4.Y = scalef4.Y * _size.height / 2 / radiu;
//
//        matrix.setrow0(AZ::Vector4::multiply(matrix.getrow0(), scalef4));
//        matrix.setrow1(AZ::Vector4::multiply(matrix.getrow1(), scalef4));
//
//        AZ::Vector4 posf4 = matrix.get_position();
//        posf4.X += _size.width * 0.5f; //(_size.width * _pivot.x - _size.width * (_pivot.x - 0.5f));
//        posf4.Y += _size.height * 0.5f; // (_size.height * _pivot.y - _size.height * (_pivot.y - 0.5f));
//        matrix.set_position(posf4);
//
//        return matrix;
    }
        
    return _transform;
}

void GGraph::RefreshTransform()
{
    GObject::RefreshTransform();

//    if (_canvasId == GFX::InvalidId)
//    {
//        _canvasId = GFX::GFXFacade::Instance()->CreateCanvas();
//    }
//
//    AZ::Transform* transform = GFX::GFXFacade::Instance()->GetCanvasTransform(_canvasId);
//    if (transform)
//    {
//        const AZ::Transform& oldM = transform->getmatrix();
//        AZ::Transform globalM = this->GetGlobalMatrix44();
//
//        AZ::Vector4 scalef4 = AZ::Vector4(RenderDataProcess::Zoom, RenderDataProcess::Zoom, 0, 1);
//        globalM.scale(scalef4);
//        AZ::Vector4 posf4 = globalM.get_position();
//        posf4.Y = RenderDataProcess::Height - posf4.Y;
//        if (_type == 1)
//            posf4.Y -= _size.height;
//
//        globalM.set_position(posf4);
//
//        transform->setmatrix(globalM);
//
//        if (globalM != oldM || _isDirty)
//        {
//            GFX::GFXFacade::Instance()->Clear(_canvasId);
//            _isDirty = false;
//
//            updateShape();
//        }
//    }
}

Vec2 GGraph::localToGlobal(const Vec2 & pt)
{
    Vec2 pt2 = pt;
    if (_pivotAsAnchor)
    {
        pt2.x += _size.width*_pivot.x;
        pt2.y += _size.height*_pivot.y;
    }

    if(_type == 2)
    {
        pt2.x -= _size.width * 0.5f;
        pt2.y -= _size.height * 0.5f;
    }

    pt2 = this->convertToWorldSpace(pt2);
    return pt2;
}
NS_FGUI_END