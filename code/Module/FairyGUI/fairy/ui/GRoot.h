#ifndef __GROOT_H__
#define __GROOT_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "event/InputProcessor.h"
#include "GComponent.h"
#include "GGraph.h"
#include "Window.h"
#include "AzCore/RTTI/TypeInfo.h"

NS_FGUI_BEGIN

class WeakPtr;

class GRoot : public GComponent
{
    typedef function<void(GTextInput*)> ShowKeyBoardCallBack;
    typedef function<void(GTextInput*)> HideKeyBoardCallBack;
    FUI_RTTI_DERIVED(GRoot, GComponent)
public:
    GRoot();
    virtual ~GRoot();

    static GRoot* create(int zOrder = 1000);
    //static GRoot* create(Scene* scene, int zOrder = 1000);
    static GRoot* getInstance() { return _inst; }

    static void destroy();

    void showWindow(Window* win);
    void hideWindow(Window* win);
    void hideWindowImmediately(Window* win);
    void bringToFront(Window* win);
    void showModalWait();
    void closeModalWait();
    void closeAllExceptModals();
    void closeAllWindows();
    Window* getTopWindow();

    GObject* getModalWaitingPane();
    GGraph* getModalLayer();
    bool hasModalWindow();
    bool isModalWaiting();

    InputProcessor* getInputProcessor() const { return _inputProcessor; }
    Vec2 getTouchPosition(int touchId);
    GObject* getTouchTarget();

    void showPopup(GObject* popup);
    void showPopup(GObject* popup, GObject* target, PopupDirection dir);
    void togglePopup(GObject* popup);
    void togglePopup(GObject* popup, GObject* target, PopupDirection dir);
    void hidePopup();
    void hidePopup(GObject* popup);
    bool hasAnyPopup();
    Vec2 getPoupPosition(GObject* popup, GObject* target, PopupDirection dir);

    void showTooltips(const AZStd::string& msg);
    void showTooltipsWin(GObject* tooltipWin);
    void hideTooltips();

    void playSound(const AZStd::string& url, float volumeScale = 1);
    bool isSoundEnabled() const { return _soundEnabled; }
    void setSoundEnabled(bool value);
    float getSoundVolumeScale() const { return _soundVolumeScale; }
    void setSoundVolumeScale(float value);

    void setKeyBoardFunc(ShowKeyBoardCallBack showFunc, HideKeyBoardCallBack hideFunc);
protected:
    virtual void handlePositionChanged() override;
    virtual void onEnter() override;
    virtual void onExit() override;

private:
    bool initWithScene(int zOrder);
    void onKeyDown(EventContext * context);
    void onKeyUp(EventContext * context);
    //bool initWithScene(Scene* scene, int zOrder);
    void onWindowSizeChanged();
    void createModalLayer();
    void adjustModalLayer();
    void closePopup(GObject* target);
    void checkPopups();
    void onTouchEvent(int eventType);
    void checkFocusChanged();

    CALL_LATER_FUNC(GRoot, doShowTooltipsWin);

    //EventListener* _windowSizeListener;
    InputProcessor* _inputProcessor;

    GGraph* _modalLayer;
    GObject* _modalWaitPane;
    vector<WeakPtr> _popupStack;
    vector<WeakPtr> _justClosedPopups;
    GObject* _tooltipWin;
    GObject* _defaultTooltipWin;

    GObject* _currentFocus;
    ShowKeyBoardCallBack _showKeyBoardFunc;
    HideKeyBoardCallBack _hideKeyBoardFunc;


    static bool _soundEnabled;
    static float _soundVolumeScale;

    static GRoot* _inst;
};

NS_FGUI_END

#endif
