#include "GObject.h"
#include "GGroup.h"
#include "GList.h"
#include "GRoot.h"
#include "UIPackage.h"
#include "gears/GearXY.h"
#include "gears/GearSize.h"
#include "gears/GearColor.h"
#include "gears/GearAnimation.h"
#include "gears/GearLook.h"
#include "gears/GearText.h"
#include "gears/GearIcon.h"
#include "utils/ToolSet.h"
#include "utils/WeakPtr.h"
#include "FairyGUI/fairy/display/DisplayObject.h"

#include "GLoader.h"

NS_FGUI_BEGIN

GObject* GObject::_draggingObject = nullptr;

static Vec2 sGlobalDragStart;
static Rect sGlobalRect;
static bool sUpdateInDragging;

GObject::GObject() :
    _scale{ 1,1 },
    _sizePercentInGroup(0.0f),
    _pivotAsAnchor(false),
    _alpha(1.0f),
    _rotation(0.0f),
    _visible(true),
    _internalVisible(true),
    _handlingController(false),
    _touchable(true),
    _grayed(false),
    _finalGrayed(false),
    _draggable(false),
    _dragBounds(nullptr),
    _dragTesting(false),
    _sortingOrder(0),
    _focusable(false),
    _pixelSnapping(false),
    _group(nullptr),
    _parent(nullptr),
    _sizeImplType(0),
    _underConstruct(false),
    _gearLocked(false),
    _gears{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
    _packageItem(nullptr),
    _data(nullptr),
    _touchDisabled(false),
    _isAdoptiveChild(false),
    _weakPtrRef(0),
    _color4b(Color4B::WHITE)
{
    static uint64_t _gInstanceCounter = 1;
    _uid = _gInstanceCounter++;
    AZStd::string ss;
    ss = _uid;
    id = ss;
    _relations = Relations::create();
    _relations->_owner = this;
    FUI_SAFE_RETAIN(_relations);
}

GObject::~GObject()
{
    removeFromParent();

    for (int i = 0; i < 8; i++)
        FUI_SAFE_DELETE(_gears[i]);

    FUI_SAFE_RELEASE(_relations);
    FUI_SAFE_DELETE(_dragBounds);

    if (_weakPtrRef > 0)
        WeakPtr::markDisposed(this);
}

bool GObject::init()
{
    handleInit();

    return true;
}

void GObject::setX(float value)
{
    setPosition(value, _position.y, false, true);
}

void GObject::setY(float value)
{
    setPosition(_position.x, value, true, false);
}

void GObject::setPosition(float xv, float yv, bool ignoreX, bool ignoreY)
{
    if (!ignoreX && _parent != nullptr)
    {
        xv += _parent->getMargin().left + _parent->GetAlignOffset().x;
    }

    if (!ignoreY && _parent != nullptr)
    {
        yv += _parent->getMargin().top + +_parent->GetAlignOffset().y;
    }
    if (_position.x != xv || _position.y != yv)
    {
        float dx = xv - _position.x;
        float dy = yv - _position.y;
        _position.x = xv;
        _position.y = yv;

        handlePositionChanged();

        GGroup* g = this->SafeCast<GGroup>();
        if (g != nullptr)
            g->moveChildren(dx, dy);

        updateGear(1);

        if (_parent != nullptr && _parent->SafeCast<GList>() == nullptr)
        {
            _parent->setBoundsChangedFlag();
            if (_group != nullptr)
                _group->setBoundsChangedFlag();

            dispatchEvent(UIEventType::PositionChange);
        }

        if (_draggingObject == this && !sUpdateInDragging)
            sGlobalRect = localToGlobal(Rect(Vec2::ZERO, _size));
    }
}

void GObject::setPixelSnapping(bool value)
{
    if (_pixelSnapping != value)
    {
        _pixelSnapping = value;
        handlePositionChanged();
    }
}

void GObject::setSize(float wv, float hv, bool ignorePivot /*= false*/)
{
    if (_rawSize.width != wv || _rawSize.height != hv)
    {
        _rawSize.width = wv;
        _rawSize.height = hv;
        if (wv < minSize.width)
            wv = minSize.width;
        else if (maxSize.width > 0 && wv > maxSize.width)
            wv = maxSize.width;
        if (hv < minSize.height)
            hv = minSize.height;
        else if (maxSize.height > 0 && hv > maxSize.height)
            hv = maxSize.height;
        float dWidth = wv - _size.width;
        float dHeight = hv - _size.height;
        _size.width = wv;
        _size.height = hv;

        handleSizeChanged();

        if (_pivot.x != 0 || _pivot.y != 0)
        {
            if (!_pivotAsAnchor)
            {
                if (!ignorePivot)
                    setPosition(_position.x - _pivot.x * dWidth, _position.y - _pivot.y * dHeight);
                else
                    handlePositionChanged();
            }
            else
                handlePositionChanged();
        }
        else
            handlePositionChanged();

        GGroup* g = this->SafeCast<GGroup>();
        if (g != nullptr)
            g->resizeChildren(dWidth, dHeight);

        updateGear(2);

        if (_parent != nullptr)
        {
            _relations->onOwnerSizeChanged(dWidth, dHeight);
            _parent->setBoundsChangedFlag();
            if (_group != nullptr)
                _group->setBoundsChangedFlag(true);
        }

        dispatchEvent(UIEventType::SizeChange);
    }
}

void GObject::setSizeDirectly(float wv, float hv)
{
    _rawSize.width = wv;
    _rawSize.height = hv;
    if (wv < 0)
        wv = 0;
    if (hv < 0)
        hv = 0;
    _size.width = wv;
    _size.height = hv;
}

void GObject::center(bool restraint /*= false*/)
{
    GComponent* r;
    if (_parent != nullptr)
        r = _parent;
    else
        r = UIRoot;

    setPosition((int)((r->_size.width - _size.width) / 2), (int)((r->_size.height - _size.height) / 2));
    if (restraint)
    {
        addRelation(r, RelationType::Center_Center);
        addRelation(r, RelationType::Middle_Middle);
    }
}

void GObject::makeFullScreen()
{
    setSize(UIRoot->getWidth(), UIRoot->getHeight());
}

void GObject::setPivot(float xv, float yv, bool asAnchor)
{
    if (_pivot.x != xv || _pivot.y != yv || _pivotAsAnchor != asAnchor)
    {
        _pivot.set(xv, yv);
        _pivotAsAnchor = asAnchor;
        handlePositionChanged();
    }
}

void GObject::setScale(float xv, float yv)
{
    if (_scale.x != xv || _scale.y != yv)
    {
        _scale.x = xv;
        _scale.y = yv;
        handleScaleChanged();

        updateGear(2);
    }
}

void GObject::setSkew(float xv, float yv)
{
    if (_skew.x != xv || _skew.y != yv)
    {
        _skew.x = xv;
        _skew.y = yv;
        handleSkewChanged();
    }
}

void GObject::setRotation(float value)
{
    if (_rotation != value)
    {
        _rotation = value;
        updateGear(3);
    }
}

void GObject::setAlpha(float value)
{
    if (_alpha != value)
    {
        _alpha = value;
        handleAlphaChanged();
        updateGear(3);
    }
}

void GObject::setGrayed(bool value)
{
    if (_grayed != value || _finalGrayed != value)
    {
        _grayed = value;
        handleGrayedChanged();
        updateGear(3);
    }
}

void GObject::setVisible(bool value)
{
    if (_visible != value)
    {
        _visible = value;
        handleVisibleChanged();
        if (this->m_displayObject)
        {
            this->m_displayObject->ChangeVisibleState(value);
        }
        if (_parent != nullptr)
            _parent->setBoundsChangedFlag();
    }
}

bool GObject::internalVisible() const
{
    return _internalVisible && (_group == nullptr || _group->internalVisible());
}

bool GObject::internalVisible2() const
{
    return _visible && (_group == nullptr || _group->internalVisible2());
}

void GObject::setTouchable(bool value)
{
    _touchable = value;
}

void GObject::setSortingOrder(int value)
{
    if (value < 0)
        value = 0;
    if (_sortingOrder != value)
    {
        int old = _sortingOrder;
        _sortingOrder = value;
        if (_parent != nullptr)
            _parent->childSortingOrderChanged(this, old, _sortingOrder);
    }
}

void GObject::setGroup(GGroup * value)
{
    if (_group != value)
    {
        if (_group != nullptr)
            _group->setBoundsChangedFlag(true);
        _group = value;
        if (_group != nullptr)
            _group->setBoundsChangedFlag(true);
        handleVisibleChanged();
        if (_parent)
            _parent->childStateChanged(this);
    }
}

const AZStd::string& GObject::getText() const
{
    return UIConfig::getInstance()->emptyString;
}

void GObject::setText(const AZStd::string & text)
{
}

const AZStd::string & GObject::getIcon() const
{
    return UIConfig::getInstance()->emptyString;
}

void GObject::setIcon(const AZStd::string & text)
{
}

void GObject::setTooltips(const AZStd::string & value)
{
    _tooltips = value;
    if (!_tooltips.empty())
    {
        addEventListener(UIEventType::RollOver, FUI_CALLBACK_1(GObject::onRollOver, this), EventTag(this));
        addEventListener(UIEventType::RollOut, FUI_CALLBACK_1(GObject::onRollOut, this), EventTag(this));
    }
}

void GObject::onRollOver(EventContext* context)
{
    getRoot()->showTooltips(_tooltips);
}

void GObject::onRollOut(EventContext* context)
{
    getRoot()->hideTooltips();
}

void GObject::setDraggable(bool value)
{
    if (_draggable != value)
    {
        _draggable = value;
        initDrag();
    }
}

void GObject::setDragBounds(const Rect & value)
{
    if (_dragBounds == nullptr)
        _dragBounds = new Rect();
    *_dragBounds = value;
}

void GObject::startDrag(int touchId)
{
    dragBegin(touchId);
}

void GObject::stopDrag()
{
    dragEnd();
}

AZStd::string GObject::getResourceURL() const
{
    if (_packageItem != nullptr)
        return "ui://" + _packageItem->owner->getId() + _packageItem->id;
    else
        return UIConfig::getInstance()->emptyString;
}

Vec2 GObject::localToGlobal(const Vec2 & pt)
{
    Vec2 pt2 = pt;
    pt2 = this->convertToWorldSpace(pt2);
    return pt2;
}

Rect GObject::localToGlobal(const Rect & rect)
{
    Rect ret;
    Vec2 v = localToGlobal(rect.origin);
    ret.origin.x = v.x;
    ret.origin.y = v.y;
    v = localToGlobal(Vec2(rect.getMaxX(), rect.getMaxY()));
    ret.size.width = v.x - ret.origin.x;
    ret.size.height = v.y - ret.origin.y;
    return ret;
}

Vec2 GObject::globalToLocal(const Vec2 & pt)
{
    Vec2 pt2 = pt;
    pt2 = this->convertToNodeSpace(pt2);
    return pt2;
}

Rect GObject::globalToLocal(const Rect & rect)
{
    Rect ret;
    Vec2 v = globalToLocal(rect.origin);
    ret.origin.x = v.x;
    ret.origin.y = v.y;
    v = globalToLocal(Vec2(rect.getMaxX(), rect.getMaxY()));
    ret.size.width = v.x - ret.origin.x;
    ret.size.height = v.y - ret.origin.y;
    return ret;
}

Rect GObject::transformRect(const Rect& rect, GObject * targetSpace)
{
    if (targetSpace == this)
        return rect;

    if (targetSpace == _parent) // optimization
    {
        return Rect((_position.x + rect.origin.x) * _scale.x,
            (_position.y + rect.origin.y) * _scale.y,
            rect.size.width * _scale.x,
            rect.size.height * _scale.y);
    }
    else
    {
        float result[4]{ FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX };

        transformRectPoint(rect.origin, result, targetSpace);
        transformRectPoint(Vec2(rect.getMaxX(), rect.origin.y), result, targetSpace);
        transformRectPoint(Vec2(rect.origin.x, rect.getMaxY()), result, targetSpace);
        transformRectPoint(Vec2(rect.getMaxX(), rect.getMaxY()), result, targetSpace);

        return Rect(result[0], result[1], result[2] - result[0], result[3] - result[1]);
    }
}

void GObject::transformRectPoint(const Vec2& pt, float rect[], GObject* targetSpace)
{
    Vec2 v = localToGlobal(pt);
    if (targetSpace != nullptr)
        v = targetSpace->globalToLocal(v);

    if (rect[0] > v.x) rect[0] = v.x;
    if (rect[2] < v.x) rect[2] = v.x;
    if (rect[1] > v.y) rect[1] = v.y;
    if (rect[3] < v.y) rect[3] = v.y;
}

Vec2 GObject::convertToWorldSpace(const Vec2 & pt)
{
    AZ::Transform m = this->GetGlobalMatrix44();
    AZ::Vector3 pos = AZ::Vector3(pt.x, pt.y, 1);
    pos = m * pos;
    return Vec2(pos.GetX(), pos.GetY());
}

Vec2 GObject::convertToNodeSpace(const Vec2 & pt)
{
    AZ::Transform m = this->GetGlobalMatrix44();
    m = m.GetInverseFull();
    AZ::Vector3 pos = AZ::Vector3(pt.x, pt.y, 1);
    pos = m * pos;
    return Vec2(pos.GetX(), pos.GetY());
}

void GObject::addRelation(GObject * target, RelationType relationType, bool usePercent)
{
    _relations->add(target, relationType, usePercent);
}

void GObject::removeRelation(GObject * target, RelationType relationType)
{
    _relations->remove(target, relationType);
}

GearBase* GObject::getGear(int index)
{
    GearBase* gear = _gears[index];
    if (gear == nullptr)
    {
        switch (index)
        {
        case 0:
            gear = new GearDisplay(this);
            break;
        case 1:
            gear = new GearXY(this);
            break;
        case 2:
            gear = new GearSize(this);
            break;
        case 3:
            gear = new GearLook(this);
            break;
        case 4:
            gear = new GearColor(this);
            break;
        case 5:
            gear = new GearAnimation(this);
            break;
        case 6:
            gear = new GearText(this);
            break;
        case 7:
            gear = new GearIcon(this);
            break;
        }
        _gears[index] = gear;
    }
    return gear;
}

void GObject::updateGear(int index)
{
    if (_underConstruct || _gearLocked)
        return;

    GearBase* gear = _gears[index];
    if (gear != nullptr && gear->getController() != nullptr)
        gear->updateState();
}

bool GObject::checkGearController(int index, GController* c)
{
    return _gears[index] != nullptr && _gears[index]->getController() == c;
}

void GObject::updateGearFromRelations(int index, float dx, float dy)
{
    if (_gears[index] != nullptr)
        _gears[index]->updateFromRelations(dx, dy);
}

uint32_t GObject::addDisplayLock()
{
    GearDisplay* gearDisplay = (GearDisplay*)_gears[0];
    if (gearDisplay != nullptr && gearDisplay->getController() != nullptr)
    {
        uint32_t ret = gearDisplay->addLock();
        checkGearDisplay();

        return ret;
    }
    else
        return 0;
}

void GObject::releaseDisplayLock(uint32_t token)
{
    GearDisplay* gearDisplay = (GearDisplay*)_gears[0];
    if (gearDisplay != nullptr && gearDisplay->getController() != nullptr)
    {
        gearDisplay->releaseLock(token);
        checkGearDisplay();
    }
}

void GObject::checkGearDisplay()
{
    if (_handlingController)
        return;

    bool connected = _gears[0] == nullptr || ((GearDisplay*)_gears[0])->isConnected();
    if (connected != _internalVisible)
    {
        _internalVisible = connected;
        if (_parent != nullptr)
            _parent->childStateChanged(this);
    }
}

GRoot* GObject::getRoot() const
{
    GObject* p = (GObject*)this;
    while (p->_parent != nullptr)
        p = p->_parent;

    GRoot* root = p->SafeCast<GRoot>();
    if (root != nullptr)
        return root;
    else
        return UIRoot;
}

void GObject::removeFromParent()
{
    if (_parent != nullptr)
        _parent->removeChild(this);
}

void GObject::constructFromResource()
{
}

GObject* GObject::hitTest(const Vec2 &worldPoint)
{
    if (_touchDisabled || !_touchable)
        return nullptr;

    Rect rect;
    rect.size = _size;

    if (GLoader* pl = this->SafeCast<GLoader>())
    {
        rect.size = pl->getContentSize();
    }

    if (rect.containsPoint(this->convertToNodeSpace(worldPoint)))
        return this;
    else
        return nullptr;
}

void GObject::handleInit()
{
}

void GObject::onEnter()
{
    dispatchEvent(UIEventType::Enter);
}

void GObject::onExit()
{
    dispatchEvent(UIEventType::Exit);
}

void GObject::handlePositionChanged()
{

}

void GObject::handleSizeChanged()
{

}

void GObject::handleScaleChanged()
{
}

void GObject::handleAlphaChanged()
{
}

void GObject::handleGrayedChanged()
{
    _finalGrayed = (_parent && _parent->_finalGrayed) || _grayed;
}

void GObject::handleVisibleChanged()
{
}

void GObject::handleControllerChanged(GController * c)
{
    _handlingController = true;
    for (int i = 0; i < 8; i++)
    {
        GearBase* gear = _gears[i];
        if (gear != nullptr && gear->getController() == c)
            gear->apply();
    }
    _handlingController = false;

    checkGearDisplay();
}

void GObject::setup_BeforeAdd(TXMLElement * xml)
{
    const char *p;
    Vec2 v2;
    Vec4 v4;

    p = xml->Attribute("id");
    if (p)
        id = p;

    p = xml->Attribute("name");
    if (p)
        name = p;

    p = xml->Attribute("xy");
    if (p)
    {
        ToolSet::splitString(p, ',', v2, true);
        setPosition(v2.x, v2.y);
    }

    p = xml->Attribute("size");
    if (p)
    {
        ToolSet::splitString(p, ',', v2, true);
        initSize = v2;
        setSize(initSize.width, initSize.height, true);
    }

    p = xml->Attribute("restrictSize");
    if (p)
    {
        ToolSet::splitString(p, ',', v4, true);
        minSize.width = v4.x;
        minSize.height = v4.z;
        maxSize.width = v4.y;
        maxSize.height = v4.w;
    }

    p = xml->Attribute("scale");
    if (p)
    {
        ToolSet::splitString(p, ',', v2);
        setScale(v2.x, v2.y);
    }

    p = xml->Attribute("skew");
    if (p)
    {
        ToolSet::splitString(p, ',', v2);
        setSkewX(v2.x);
        setSkewY(v2.y);
    }

    p = xml->Attribute("rotation");
    if (p)
        setRotation(atoi(p));

    p = xml->Attribute("pivot");
    if (p)
    {
        ToolSet::splitString(p, ',', v2);
        setPivot(v2.x, v2.y, xml->BoolAttribute("anchor"));
    }

    p = xml->Attribute("alpha");
    if (p)
        setAlpha(atof(p));

    p = xml->Attribute("touchable");
    if (p)
        setTouchable(strcmp(p, "true") == 0);

    p = xml->Attribute("visible");
    if (p)
        setVisible(strcmp(p, "true") == 0);

    p = xml->Attribute("grayed");
    if (p)
        setGrayed(strcmp(p, "true") == 0);

    p = xml->Attribute("tooltips");
    if (p)
        setTooltips(p);
}

void GObject::setup_AfterAdd(TXMLElement * xml)
{
    const char *p;

    p = xml->Attribute("group");
    if (p)
        _group = _parent->getChildById(p)->SafeCast<GGroup>();

    TXMLElement* exml = xml->FirstChildElement();
    while (exml)
    {
        int gearIndex = ToolSet::parseGearIndex(exml->Name());
        if (gearIndex != -1)
            getGear(gearIndex)->setup(exml);

        exml = exml->NextSiblingElement();
    }
}

void GObject::initDrag()
{
    if (_draggable)
    {
        addEventListener(UIEventType::TouchBegin, FUI_CALLBACK_1(GObject::onTouchBegin, this), EventTag(this));
        addEventListener(UIEventType::TouchMove, FUI_CALLBACK_1(GObject::onTouchMove, this), EventTag(this));
        addEventListener(UIEventType::TouchEnd, FUI_CALLBACK_1(GObject::onTouchEnd, this), EventTag(this));
    }
    else
    {
        removeEventListener(UIEventType::TouchBegin, EventTag(this));
        removeEventListener(UIEventType::TouchMove, EventTag(this));
        removeEventListener(UIEventType::TouchEnd, EventTag(this));
    }
}

void GObject::dragBegin(int touchId)
{
    if (_draggingObject != nullptr)
    {
        GObject* tmp = _draggingObject;
        _draggingObject->stopDrag();
        _draggingObject = nullptr;
        tmp->dispatchEvent(UIEventType::DragEnd);
    }

    sGlobalDragStart = UIRoot->getTouchPosition(touchId);
    sGlobalRect = localToGlobal(Rect(Vec2::ZERO, _size));

    _draggingObject = this;
    _dragTesting = true;
    UIRoot->getInputProcessor()->addTouchMonitor(touchId, this);

    addEventListener(UIEventType::TouchMove, FUI_CALLBACK_1(GObject::onTouchMove, this), EventTag(this));
    addEventListener(UIEventType::TouchEnd, FUI_CALLBACK_1(GObject::onTouchEnd, this), EventTag(this));
}

void GObject::dragEnd()
{
    if (_draggingObject == this)
    {
        _dragTesting = false;
        _draggingObject = nullptr;
    }
}

void GObject::onTouchBegin(EventContext* context)
{
    _dragTouchStartPos = context->getInput()->getPosition();
    _dragTesting = true;
    context->captureTouch();
}

void GObject::onTouchMove(EventContext* context)
{
    InputEvent* evt = context->getInput();

    if (_draggingObject != this && _draggable && _dragTesting)
    {
        int sensitivity;
#ifdef CC_PLATFORM_PC
        sensitivity = UIConfig::getInstance()->clickDragSensitivity;
#else
        sensitivity = UIConfig::getInstance()->touchDragSensitivity;
#endif 
        if (abs(_dragTouchStartPos.x - evt->getPosition().x) < sensitivity
            && abs(_dragTouchStartPos.y - evt->getPosition().y) < sensitivity)
            return;

        _dragTesting = false;
        if (!dispatchEvent(UIEventType::DragStart))
            dragBegin(evt->getTouchId());
    }

    if (_draggingObject == this)
    {
        float xx = evt->getPosition().x - sGlobalDragStart.x + sGlobalRect.origin.x;
        float yy = evt->getPosition().y - sGlobalDragStart.y + sGlobalRect.origin.y;

        if (_dragBounds != nullptr)
        {
            Rect rect = UIRoot->localToGlobal(*_dragBounds);
            if (xx < rect.origin.x)
                xx = rect.origin.x;
            else if (xx + sGlobalRect.size.width > rect.getMaxX())
            {
                xx = rect.getMaxX() - sGlobalRect.size.width;
                if (xx < rect.origin.x)
                    xx = rect.origin.x;
            }

            if (yy < rect.origin.y)
                yy = rect.origin.y;
            else if (yy + sGlobalRect.size.height > rect.getMaxY())
            {
                yy = rect.getMaxY() - sGlobalRect.size.height;
                if (yy < rect.origin.y)
                    yy = rect.origin.y;
            }
        }

        Vec2 pt = _parent->globalToLocal(Vec2(xx, yy));

        sUpdateInDragging = true;
        setPosition(round(pt.x), round(pt.y));
        sUpdateInDragging = false;

        dispatchEvent(UIEventType::DragMove);
    }
}

void GObject::onTouchEnd(EventContext* context)
{
    if (_draggingObject == this)
    {
        _draggingObject = nullptr;
        dispatchEvent(UIEventType::DragEnd);
    }
}

void GObject::RefreshSelfTransform()
{
    using namespace AZ;
 
    Size s = _size;
    if (GLoader* pl = this->SafeCast<GLoader>())
    {
        s = pl->getContentSize();
    }
    
    Matrix4x4 res = Matrix4x4::CreateIdentity();
    Matrix4x4 scaleCenterNegative = Matrix4x4::CreateIdentity();
    Matrix4x4 scaleRotation = Matrix4x4::CreateIdentity();
    Matrix4x4 scaleRotationT = Matrix4x4::CreateIdentity();
    Matrix4x4 scale = Matrix4x4::CreateIdentity();
    Matrix4x4 rot = Matrix4x4::CreateIdentity();

    Vector4 vecScalingCenter(_pivot.x * s.width, _pivot.y * s.height, 0.0f, 0.0f);
    Vector4 negVecScalingCenter(-_pivot.x * s.width, -_pivot.y * s.height, 0.0f, 0.0f);
    Vector4 vecRotationCenter(_pivot.x * s.width, _pivot.y * s.height, 0.0f, 0.0f);
    Vector4 vecTranslation(_position.x, _position.y, 0.0f, 0.0f);
    if (_pivotAsAnchor)
    {
         vecTranslation = Vector4(_position.x - _pivot.x * s.width, _position.y - _pivot.y * s.height, 0.0f, 0.0f);
    }

    scaleCenterNegative.SetRow(3, Vector4(negVecScalingCenter.GetX(), negVecScalingCenter.GetY(), negVecScalingCenter.GetZ(),1));
    scaleRotation = Matrix4x4::CreateFromQuaternion(AZ::Quaternion::CreateIdentity());
    scaleRotationT = scaleRotation.GetTranspose();

    scale = Matrix4x4::CreateScale(AZ::Vector3(_scale.x, _scale.y, 1));
    rot = Matrix4x4::CreateFromQuaternion(AZ::Quaternion::CreateRotationZ(-_rotation * N_PI / 180.0f));

    res = scaleCenterNegative * scaleRotationT;
    res = res * scale;
    res = res * scaleRotation;

    res.SetRow(3, res.GetRow(3) + vecScalingCenter - vecRotationCenter);
    res = res * rot;
    res.SetRow(3, res.GetRow(3) + vecRotationCenter + vecTranslation);

    Matrix4x4 resT = res.GetTranspose();

    _transform.SetRows(resT.GetRow(0), resT.GetRow(1), resT.GetRow(2));
}


void GObject::RefreshTransform()
{
    /*
    Size s = _size;
    if (GLoader* pl = this->SafeCast<GLoader>())
    {
        s = pl->getContentSize();
    }

    auto pos = AZ::Transform::CreateTranslation(AZ::Vector3(_position.x, _position.y, 0));

    if(_pivotAsAnchor)
    {
        pos = AZ::Transform::CreateTranslation(AZ::Vector3(_position.x - _pivot.x * s.width, _position.y - _pivot.y * s.height, 0));
    }
    auto scale = AZ::Transform::CreateScale(AZ::Vector3(_scale.x, _scale.y, 1));
    auto rotate = AZ::Transform::CreateFromQuaternion(AZ::Quaternion::CreateRotationZ(_rotation * N_PI / 180.0f));

    auto parent = this->getParent();
    if(parent)
    {
        if(GList* list = parent->SafeCast<GList>())
        {
            auto scroll = parent->getScrollPane();
            if(scroll != nullptr && parent->getMask() != this)
            {
                pos.SetPosition(AZ::Vector3(pos.GetPosition().GetX() + scroll->getScrollX(), pos.GetPosition().GetY() + scroll->getScrollY(), 0));
            }
        }
    }
    
    _transform = pos * scale * rotate;
    */
    RefreshSelfTransform();
    GComponent* p = this->SafeCast<GComponent>();
    if (p)
    {
        if(p->getMask())
        {
            p->getMask()->RefreshTransform();
        }
        for (auto element : p->getChildren())
        {
            element->RefreshTransform();
        }
    }
}

AZ::Transform* GObject::GetTransform()
{
    return &_transform;
}

AZ::Transform GObject::GetLocalMatrix44()
{
    return _transform;
}

AZ::Transform GObject::GetGlobalMatrix44()
{
    AZ::Transform result = GetLocalMatrix44();
    GComponent* pParent = this->getParent();
    while(pParent)
    {
        result = pParent->GetLocalMatrix44() * result;
        pParent = pParent->getParent();
    }
    return result;
}

NS_FGUI_END