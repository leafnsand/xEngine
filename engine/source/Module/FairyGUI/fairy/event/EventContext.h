#ifndef __EVENTCONTEXT_H__
#define __EVENTCONTEXT_H__

#include "FairyGUIMacros.h"

#include "InputEvent.h"

#include "base/Base.h"

NS_FGUI_BEGIN

class GObject;
class InputProcessor;

class EventContext : public Ref
{
    FUI_RTTI_DERIVED(EventContext, Ref)
public:
    EventContext();
    ~EventContext();

    CREATE_FUNC(EventContext);

    int getType() const { return _type; }
    Ref* getSender() const { return _sender; }
    InputEvent* getInput() const { return _inputEvent; }
    void stopPropagation() { _isStopped = true; }
    void preventDefault() { _defaultPrevented = true; }
    bool isDefaultPrevented() { return _defaultPrevented; }
    void captureTouch() { _touchCapture = 1; }
    void uncaptureTouch() { _touchCapture = 2; }

    const Value& getDataValue() const { return _dataValue; }
    void* getData() const { return _data; }

private:
    bool init() { return true; }

    Ref* _sender;
    InputEvent* _inputEvent;
    Value _dataValue;
    void* _data;
    bool _isStopped;
    bool _defaultPrevented;
    int _touchCapture;
    int _type;

    friend class UIEventDispatcher;
};

NS_FGUI_END

#endif
