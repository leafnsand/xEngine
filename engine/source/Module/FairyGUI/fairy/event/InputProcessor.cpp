#include "InputProcessor.h"
#include "InputEvent.h"
#include "ui/GComponent.h"
#include "ui/GRoot.h"
#include "ui/GRichTextField.h"
#include "ui/GTextInput.h"
#include "utils/WeakPtr.h"
#include "utils/FUISystemUtils.h"
#include "render/RenderDataProcess.h"

NS_FGUI_BEGIN


InputProcessor* InputProcessor::_activeProcessor = nullptr;
bool InputProcessor::_touchOnUI = false;
unsigned int InputProcessor::_touchOnUIFlagFrameId = 0;

class TouchInfo
{
public:
    TouchInfo();
    ~TouchInfo();

    void reset();

    Touch* touch;
    Vec2 pos;
    int touchId;
    int clickCount;
    int mouseWheelDelta;
    MouseButton button;
    Vec2 downPos;
    bool began;
    bool clickCancelled;
    clock_t lastClickTime;
    WeakPtr lastRollOver;
    vector<WeakPtr> downTargets;
    vector<WeakPtr> touchMonitors;
};

InputProcessor::InputProcessor(GComponent* owner) :
    _keyModifiers(0)
{
    _owner = owner;
    _recentInput._inputProcessor = this;
}

InputProcessor::~InputProcessor()
{
    for (auto &ti : _touches)
        delete ti;
}

Vec2 InputProcessor::getTouchPosition(int touchId)
{
    for (auto &ti : _touches)
    {
        if (ti->touchId == touchId)
            return ti->pos;
    }
    return _recentInput.getPosition();
}

TouchInfo* InputProcessor::getTouch(int touchId, bool createIfNotExisits)
{
    TouchInfo* ret = nullptr;
    for (auto &ti : _touches)
    {
        if (ti->touchId == touchId)
            return ti;
        else if (ti->touchId == -1)
            ret = ti;
    }

    if (!ret)
    {
        if (!createIfNotExisits)
            return nullptr;

        ret = new TouchInfo();
        _touches.push_back(ret);
    }
    ret->touchId = touchId;
    return ret;
}

void InputProcessor::updateRecentInput(TouchInfo* ti, GObject* target)
{
    _recentInput._pos.x = (int)ti->pos.x;
    _recentInput._pos.y = (int)ti->pos.y;
    if(_recentInput._target != target)
        _recentInput._target = target;
    _recentInput._clickCount = ti->clickCount;
    _recentInput._button = ti->button;
    _recentInput._mouseWheelDelta = ti->mouseWheelDelta;
    _recentInput._touch = ti->touch;
    _recentInput._touchId = ti->touch ? ti->touchId : -1;

    int curFrame = FUISystemUtils::getInstance()->getTotalFrames();
    bool flag = target != _owner;
    if (curFrame != _touchOnUIFlagFrameId)
        _touchOnUI = flag;
    else if (flag)
        _touchOnUI = true;
    _touchOnUIFlagFrameId = curFrame;
}

void InputProcessor::handleRollOver(TouchInfo* touch, GObject* target)
{
    if (touch->lastRollOver == target)
        return;

    vector<WeakPtr> rollOutChain;
    vector<WeakPtr> rollOverChain;
    GObject* element = touch->lastRollOver.ptr();
    while (element != nullptr)
    {
        rollOutChain.push_back(WeakPtr(element));
        element = element->getParent();
    }

    element = target;
    while (element != nullptr)
    {
        auto iter = find(rollOutChain.cbegin(), rollOutChain.cend(), element);
        if (iter != rollOutChain.cend())
        {
            rollOutChain.resize(iter - rollOutChain.cbegin());
            break;
        }
        rollOverChain.push_back(WeakPtr(element));

        element = element->getParent();
    }

    touch->lastRollOver = target;

    for (auto &wptr : rollOutChain)
    {
        element = wptr.ptr();
        if (element)
            element->dispatchEvent(UIEventType::RollOut);
    }

    for (auto &wptr : rollOverChain)
    {
        element = wptr.ptr();
        if (element)
            element->dispatchEvent(UIEventType::RollOver);
    }
}

void InputProcessor::addTouchMonitor(int touchId, GObject * target)
{
    TouchInfo* ti = getTouch(touchId, false);
    if (!ti)
        return;

    auto it = find(ti->touchMonitors.cbegin(), ti->touchMonitors.cend(), target);
    if (it == ti->touchMonitors.cend())
        ti->touchMonitors.push_back(WeakPtr(target));
}

void InputProcessor::removeTouchMonitor(GObject * target)
{
    for (auto it = _touches.cbegin(); it != _touches.cend(); ++it)
    {
        auto it2 = find((*it)->touchMonitors.begin(), (*it)->touchMonitors.end(), target);
        if (it2 != (*it)->touchMonitors.end())
            *it2 = nullptr;
    }
}

void InputProcessor::cancelClick(int touchId)
{
    TouchInfo* ti = getTouch(touchId, false);
    if (ti)
        ti->clickCancelled = true;
}

void InputProcessor::setBegin(TouchInfo* touch, GObject* target)
{
    touch->began = true;
    touch->clickCancelled = false;
    touch->downPos = touch->pos;

    touch->downTargets.clear();
    GObject* obj = target;
    while (obj != nullptr)
    {
        touch->downTargets.push_back(WeakPtr(obj));
        obj = obj->getParent();
    }
}

void InputProcessor::setEnd(TouchInfo* touch, GObject* target)
{
    touch->began = false;

    auto now = clock();
    float elapsed = (now - touch->lastClickTime) / (double)CLOCKS_PER_SEC;

    if (elapsed < 0.45f)
    {
        if (touch->clickCount == 2)
            touch->clickCount = 1;
        else
            touch->clickCount++;
    }
    else
        touch->clickCount = 1;
    touch->lastClickTime = now;
}

GObject* InputProcessor::clickTest(TouchInfo* touch, GObject* target)
{
    if (touch->downTargets.empty()
        || touch->clickCancelled
        || abs(touch->pos.x - touch->downPos.x) > 50 || abs(touch->pos.y - touch->downPos.y) > 50)
        return nullptr;

    GObject* obj = touch->downTargets[0].ptr();
    if (obj)
        return obj;

    obj = target;
    while (obj != nullptr)
    {
        auto it = find(touch->downTargets.cbegin(), touch->downTargets.cend(), obj);
        if (it != touch->downTargets.cend())
        {
            obj = it->ptr();
            break;
        }

        obj = obj->getParent();
    }

    return obj;
}

bool InputProcessor::isTouchOnUI()
{
    return _touchOnUI;
}

bool InputProcessor::onTouchBegan(Touch *touch)
{
    Vec2 pt = touch->getLocation();
    pt = pt - RenderDataProcess::OffSet;
    GObject* target = _owner->hitTest(pt);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(touch->getID());
    ti->pos = Vec2(pt.x, pt.y);// UIRoot->getHeight() - pt.y);
    ti->button = MouseButton::BUTTON_LEFT;
    ti->touch = touch;
    setBegin(ti, target);

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchBegin);

    WeakPtr wptr(target);
    target->bubbleEvent(UIEventType::TouchBegin);
    target = wptr.ptr();

    handleRollOver(ti, target);

    _activeProcessor = nullptr;

    return true;
}

void InputProcessor::onTouchMoved(Touch *touch)
{
    Vec2 pt = touch->getLocation();
    pt = pt - RenderDataProcess::OffSet;
    GObject* target = _owner->hitTest(pt);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(touch->getID());
    ti->pos = Vec2(pt.x, pt.y);// UIRoot->getHeight() - pt.y);
    ti->button = MouseButton::BUTTON_LEFT;
    ti->touch = touch;

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchMove);

    handleRollOver(ti, target);

    if (ti->began)
    {
        bool done = false;
        size_t cnt = ti->touchMonitors.size();
        if (cnt > 0)
        {
            for (size_t i = 0; i < cnt; i++)
            {
                GObject* mm = ti->touchMonitors[i].ptr();
                if (!mm)
                    continue;

                mm->dispatchEvent(UIEventType::TouchMove);
                if (mm == _owner)
                    done = true;
            }
        }
        if (!done)
            _owner->dispatchEvent(UIEventType::TouchMove);
    }

    _activeProcessor = nullptr;
}

void InputProcessor::onTouchEnded(Touch *touch)
{
    Vec2 pt = touch->getLocation();
    pt = pt - RenderDataProcess::OffSet;
    GObject* target = _owner->hitTest(pt);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(touch->getID());
    ti->pos = Vec2(pt.x, pt.y);// UIRoot->getHeight() - pt.y);
    ti->button = MouseButton::BUTTON_LEFT;
    ti->touch = touch;
    setEnd(ti, target);

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchEnd);

    WeakPtr wptr(target);
    size_t cnt = ti->touchMonitors.size();
    if (cnt > 0)
    {
        for (size_t i = 0; i < cnt; i++)
        {
            GObject* mm = ti->touchMonitors[i].ptr();
            if (!mm)
                continue;

            if (mm != target
                && (!mm->SafeCast<GComponent>() || !((GComponent*)mm)->isAncestorOf(target)))
                mm->dispatchEvent(UIEventType::TouchEnd);
        }
        ti->touchMonitors.clear();
        target = wptr.ptr();
    }
    if (target)
    {
        target->bubbleEvent(UIEventType::TouchEnd);
        target = wptr.ptr();
    }

    target = clickTest(ti, target);
    if (target)
    {
        wptr = target;
        updateRecentInput(ti, target);

        GRichTextField* tf = target->SafeCast<GRichTextField>();
        if (tf)
        {
            const char* linkHref = nullptr;// dynamic_cast<FUIRichText*>(tf->displayObject())->hitTestLink(pt);
            if (linkHref)
            {
                tf->bubbleEvent(UIEventType::ClickLink, nullptr, Value(linkHref));
                target = wptr.ptr();
            }
        }

        target->bubbleEvent(UIEventType::Click);
        target = wptr.ptr();
    }
    UIRoot->bubbleEvent(UIEventType::Click);

#ifndef CC_PLATFORM_PC
    //on mobile platform, trigger RollOut on up event, but not on PC
    handleRollOver(ti, nullptr);
#else
    handleRollOver(ti, target);
#endif
    ti->touchId = -1;
    ti->button = MouseButton::BUTTON_UNSET;

    _activeProcessor = nullptr;
}

void InputProcessor::onTouchCancelled(Touch* touch)
{
    TouchInfo* ti = getTouch(touch->getID());
    if (ti == nullptr)
        return;

    ti->touch = touch;
    updateRecentInput(ti, _owner);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchEnd);

    size_t cnt = ti->touchMonitors.size();
    if (cnt > 0)
    {
        for (size_t i = 0; i < cnt; i++)
        {
            GObject* mm = ti->touchMonitors[i].ptr();
            if (!mm)
                continue;

            if (mm != _owner)
                mm->dispatchEvent(UIEventType::TouchEnd);
        }
        ti->touchMonitors.clear();
    }
    _owner->dispatchEvent(UIEventType::TouchEnd);

    handleRollOver(ti, nullptr);

    ti->touchId = -1;
    ti->button = MouseButton::BUTTON_UNSET;
    _activeProcessor = nullptr;
}

void InputProcessor::onMouseDown(MouseButton button, float cursorX, float cursorY)
{
    if (button != MouseButton::BUTTON_LEFT)
        return;

    Vec2 pt(cursorX, cursorY);
    pt = pt - RenderDataProcess::OffSet;
    GObject* target = _owner->hitTest(pt);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(0);
    ti->pos = Vec2(pt.x, UIRoot->getHeight() - pt.y);
    ti->button = button;
    ti->touch = nullptr;
    setBegin(ti, target);

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchBegin);

    WeakPtr wptr(target);
    target->bubbleEvent(UIEventType::TouchBegin);

    _activeProcessor = nullptr;
}

void InputProcessor::onMouseUp(MouseButton button, float cursorX, float cursorY)
{
    if (button != MouseButton::BUTTON_LEFT)
        return;

    Vec2 pt(cursorX, cursorY);
    pt = pt - RenderDataProcess::OffSet;
    GObject* target = _owner->hitTest(pt);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(0);
    ti->pos = Vec2(pt.x, UIRoot->getHeight() - pt.y);
    ti->button = button;
    ti->touch = nullptr;
    setEnd(ti, target);

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchEnd);

    WeakPtr wptr(target);
    size_t cnt = ti->touchMonitors.size();
    if (cnt > 0)
    {
        for (size_t i = 0; i < cnt; i++)
        {
            GObject* mm = ti->touchMonitors[i].ptr();
            if (!mm)
                continue;

            if (mm != target
                && (!mm->SafeCast<GComponent>() || !((GComponent*)mm)->isAncestorOf(target)))
                mm->dispatchEvent(UIEventType::TouchEnd);
        }
        ti->touchMonitors.clear();
        target = wptr.ptr();
    }
    if (target)
    {
        target->bubbleEvent(UIEventType::TouchEnd);
        target = wptr.ptr();
    }

    target = clickTest(ti, target);
    if (target)
    {
        wptr = target;
        updateRecentInput(ti, target);

        if (ti->button == MouseButton::BUTTON_MIDDLE)
            target->bubbleEvent(UIEventType::MiddleClick);
        else
            target->bubbleEvent(UIEventType::RightClick);
    }

    ti->touchId = -1;
    ti->button = MouseButton::BUTTON_UNSET;

    _activeProcessor = nullptr;
}

void InputProcessor::onMouseMove(MouseButton button, float cursorX, float cursorY)
{
    Vec2 pt(cursorX, cursorY);
    pt = pt - RenderDataProcess::OffSet;

    TouchInfo* ti = getTouch(0);
    if (abs(ti->pos.x - pt.x) < 1
        && abs(ti->pos.y - (UIRoot->getHeight() - pt.y)) < 1)
        return;

    GObject* target = _owner->hitTest(pt);
    if (!target)
        target = _owner;

    ti->pos = Vec2(pt.x, UIRoot->getHeight() - pt.y);
    ti->touch = nullptr;

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchMove);

    handleRollOver(ti, target);

    if (ti->began)
    {
        bool done = false;
        size_t cnt = ti->touchMonitors.size();
        if (cnt > 0)
        {
            for (size_t i = 0; i < cnt; i++)
            {
                GObject* mm = ti->touchMonitors[i].ptr();
                if (!mm)
                    continue;

                mm->dispatchEvent(UIEventType::TouchMove);
                if (mm == _owner)
                    done = true;
            }
        }
        if (!done)
            _owner->dispatchEvent(UIEventType::TouchMove);
    }

    _activeProcessor = nullptr;
}

void InputProcessor::onMouseScroll(MouseButton button, float cursorX, float cursorY, float scrollX, float scrollY)
{
    Vec2 pt(cursorX, cursorY);
    pt = pt - RenderDataProcess::OffSet;
    GObject* target = _owner->hitTest(pt);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(0);
    ti->pos = Vec2(pt.x, UIRoot->getHeight() - pt.y);
    ti->touch = nullptr;
    ti->mouseWheelDelta = MAX(scrollX, scrollY);

    updateRecentInput(ti, target);
    _activeProcessor = this;

    target->bubbleEvent(UIEventType::MouseWheel);
    ti->mouseWheelDelta = 0;

    _activeProcessor = nullptr;
}

void InputProcessor::onKeyDown(KeyCode keyCode)
{
    if (keyCode == KeyCode::KEY_LEFT_CTRL || keyCode == KeyCode::KEY_RIGHT_CTRL)
        _keyModifiers |= 1;
    else if (keyCode == KeyCode::KEY_LEFT_ALT || keyCode == KeyCode::KEY_RIGHT_ALT)
        _keyModifiers |= 2;
    else if (keyCode == KeyCode::KEY_LEFT_SHIFT || keyCode == KeyCode::KEY_RIGHT_SHIFT)
        _keyModifiers |= 4;

    _activeProcessor = this;

    _recentInput._keyCode = keyCode;
    _recentInput._target = _owner;
    _recentInput._target->dispatchEvent(UIEventType::KeyDown);

    _activeProcessor = nullptr;
}

void InputProcessor::onKeyUp(KeyCode keyCode)
{
    if (keyCode == KeyCode::KEY_LEFT_CTRL || keyCode == KeyCode::KEY_RIGHT_CTRL)
        _keyModifiers &= ~1;
    else if (keyCode == KeyCode::KEY_LEFT_ALT || keyCode == KeyCode::KEY_RIGHT_ALT)
        _keyModifiers &= ~2;
    else if (keyCode == KeyCode::KEY_LEFT_SHIFT || keyCode == KeyCode::KEY_RIGHT_SHIFT)
        _keyModifiers &= ~4;

    _activeProcessor = this;

    _recentInput._keyCode = keyCode;
    _recentInput._target = _owner;
    _recentInput._target->dispatchEvent(UIEventType::KeyUp);

    _activeProcessor = nullptr;
}

TouchInfo::TouchInfo() :
    touch(nullptr),
    touchId(-1),
    clickCount(0),
    mouseWheelDelta(0),
    button(MouseButton::BUTTON_UNSET),
    began(false),
    lastClickTime(0),
    clickCancelled(false)
{
}

TouchInfo::~TouchInfo()
{
    downTargets.clear();
    touchMonitors.clear();
}

void TouchInfo::reset()
{
    touchId = -1;
    mouseWheelDelta = 0;
    button = MouseButton::BUTTON_UNSET;
    touch = nullptr;
    pos.setZero();
    downPos.setZero();
    clickCount = 0;
    lastClickTime = 0;
    began = false;
    downTargets.clear();
    lastRollOver = nullptr;
    clickCancelled = false;
    touchMonitors.clear();
}

NS_FGUI_END