#ifndef __TRANSITION_H__
#define __TRANSITION_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"

NS_FGUI_BEGIN

class GObject;
class GComponent;
class TransitionItem;
class TransitionValue;

enum class TransitionActionType
{
    XY,
    Size,
    Scale,
    Pivot,
    Alpha,
    Rotation,
    Color,
    Animation,
    Visible,
    Sound,
    Transition,
    Shake,
    ColorFilter,
    Skew,
    Unknown
};

class Transition : public Ref
{
    FUI_RTTI_DERIVED(Transition, Ref)
public:
    typedef function<void()> PlayCompleteCallback;
    typedef function<void()> TransitionHook;

    Transition(GComponent* owner, int index);
    virtual ~Transition();

    GComponent* getOwner() { return _owner; }
    bool isAutoPlay() { return _autoPlay; }
    void setAutoPlay(bool value);
    bool isPlaying() { return _playing; }

    void play(PlayCompleteCallback callback = nullptr);
    void play(int times, float delay, PlayCompleteCallback callback = nullptr);
    void playReverse(PlayCompleteCallback callback = nullptr);
    void playReverse(int times, float delay, PlayCompleteCallback callback = nullptr);
    void changeRepeat(int value);
    void stop();
    void stop(bool setToComplete, bool processCallback);

    void setValue(const AZStd::string& label, const ValueVector& values);
    void setHook(const AZStd::string& label, TransitionHook callback);
    void clearHooks();
    void setTarget(const AZStd::string& label, GObject* newTarget);
    void setDuration(const AZStd::string& label, float value);

    void updateFromRelations(const AZStd::string& targetId, float dx, float dy);
    void OnOwnerRemovedFromStage();

    void setup(TXMLElement* xml);
    void clone(const Transition& transition,const GObject& object);

    AZStd::string name;
    int autoPlayRepeat;
    float autoPlayDelay;

private:
    void play(int times, float delay, PlayCompleteCallback onComplete, bool reverse);
    void stopItem(TransitionItem* item, bool setToComplete);
    void internalPlay(float delay);
    void startTween(TransitionItem* item, float delay);
    void tweenComplete(TransitionItem* item);
    void checkAllComplete();
    void applyValue(TransitionItem* item, TransitionValue& value);
    void playTransComplete(TransitionItem* item);
    void shakeItem(float dt, TransitionItem* item);

    void decodeValue(TransitionActionType type, const char* pValue, TransitionValue& value);

    GComponent* _owner;
    vector<TransitionItem*> _items;
    int _totalTimes;
    int _totalTasks;
    bool _playing;
    float _ownerBaseX;
    float _ownerBaseY;
    PlayCompleteCallback _onComplete;
    int _options;
    bool _reversed;
    float _maxTime;
    bool _autoPlay;
    float _timeScale;
    int _actionTag;
};

NS_FGUI_END

#endif
