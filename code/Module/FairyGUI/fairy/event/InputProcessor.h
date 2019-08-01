#ifndef __INPUTPROCESSOR_H__
#define __INPUTPROCESSOR_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"
#include "UIEventDispatcher.h"
#include "InputEvent.h"

NS_FGUI_BEGIN

class GComponent;
class TouchInfo;

class InputProcessor
{
public:
    typedef function<void(int eventType)> CaptureEventCallback;

    InputEvent* getRecentInput() { return &_recentInput; }
    static bool isTouchOnUI();

    InputProcessor(GComponent* owner);
    ~InputProcessor();

    Vec2 getTouchPosition(int touchId);

    void addTouchMonitor(int touchId, GObject* target);
    void removeTouchMonitor(GObject* target);

    void cancelClick(int touchId);

    void setCaptureCallback(CaptureEventCallback value) { _captureCallback = value; }

public:
    void onMouseScroll(MouseButton button, float cursorX, float cursorY, float scrollX, float scrollY);

    void onKeyDown(KeyCode keyCode);
    void onKeyUp(KeyCode keyCode);

    bool onTouchBegan(Touch * touch);
    void onTouchMoved(Touch * touch);
    void onTouchEnded(Touch * touch);
    void onTouchCancelled(Touch * touch);

    void onMouseDown(MouseButton button, float cursorX, float cursorY);
    void onMouseUp(MouseButton button, float cursorX, float cursorY);
    void onMouseMove(MouseButton button, float cursorX, float cursorY);

private:

    TouchInfo* getTouch(int touchId, bool createIfNotExisits = true);
    void updateRecentInput(TouchInfo* touch, GObject* target);
    void handleRollOver(TouchInfo* touch, GObject* target);
    void setBegin(TouchInfo* touch, GObject* target);
    void setEnd(TouchInfo* touch, GObject* target);
    GObject* clickTest(TouchInfo* touch, GObject* target);

    vector<TouchInfo*> _touches;
    GComponent* _owner;
    CaptureEventCallback _captureCallback;
    InputEvent _recentInput;
    uint16_t _keyModifiers;

    static bool _touchOnUI;
    static unsigned int _touchOnUIFlagFrameId;
    static InputProcessor* _activeProcessor;

    friend class UIEventDispatcher;
};

NS_FGUI_END

#endif
