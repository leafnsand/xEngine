#ifndef __SCROLLPANE_H__
#define __SCROLLPANE_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "Margin.h"
#include <ctime>
#include "FieldTypes.h"

NS_FGUI_BEGIN

class GObject;
class GComponent;
class GScrollBar;
class GController;
class EventContext;

class ScrollPane : public Ref
{
    FUI_RTTI_DERIVED(ScrollPane, Ref)
public:
    ScrollPane(GComponent* owner,
        ScrollType scrollType,
        const Margin& scrollBarMargin,
        ScrollBarDisplayType scrollBarDisplay,
        int flags,
        const AZStd::string& vtScrollBarRes,
        const AZStd::string& hzScrollBarRes,
        const AZStd::string& headerRes,
        const AZStd::string& footerRes);
    virtual ~ScrollPane();

    GComponent* getOwner() const { return _owner; }
    GComponent* getHeader() const { return _header; }
    GComponent* getFooter() const { return _footer; }
    GScrollBar* getVtScrollBar() const { return _vtScrollBar; }
    GScrollBar* getHzScrollBar() const { return _hzScrollBar; }

    bool isBouncebackEffect() const { return _bouncebackEffect; }
    void setBouncebackEffect(bool value) { _bouncebackEffect = value; }

    bool isTouchEffect() const { return _touchEffect; }
    void setTouchEffect(bool value) { _touchEffect = value; }

    bool isInertiaDisabled() const { return _inertiaDisabled; }
    void setInertiaDisabled(bool value) { _inertiaDisabled = value; }

    float getScrollStep() const { return _scrollStep; }
    void setScrollStep(float value);

    bool isSnapToItem() const { return _snapToItem; }
    void setSnapToItem(bool value) { _snapToItem = value; }

    bool isPageMode() const { return _pageMode; }
    void setPageMode(bool value) { _pageMode = value; }

    GController* getPageController() const { return _pageController; }
    void setPageController(GController* value) { _pageController = value; }

    bool isMouseWheelEnabled() const { return _mouseWheelEnabled; }
    void setMouseWheelEnabled(bool value) { _mouseWheelEnabled = value; }

    float getDecelerationRate() const { return _decelerationRate; }
    void setDecelerationRate(float value) { _decelerationRate = value; }

    float getPosX() const { return _xPos; }
    void setPosX(float value, bool ani = false);
    float getPosY() const { return _yPos; }
    void setPosY(float value, bool ani = false);
    Vec2 getPos() { return Vec2(_xPos, _yPos); }
    void setPos(float valueX, float valueY) { _xPos = valueX; _yPos = valueY; }
    void setPos(Vec2 value) { _xPos = value.x; _yPos = value.y; }

    float getPercX() const;
    void setPercX(float value, bool ani = false);
    float getPercY() const;
    void setPercY(float value, bool ani = false);

    float getScrollX() const { return _scrollX; }
    void setScrollX(float value);
    float getScrollY() const { return _scrollY; }
    void setScrollY(float value);
    Vec2 getScrollPos() { return Vec2(_scrollX, _scrollY); }
    void setScrollPos(float x, float y);

    bool isBottomMost() const;
    bool isRightMost() const;

    void scrollLeft(float ratio = 1, bool ani = false);
    void scrollRight(float ratio = 1, bool ani = false);
    void scrollUp(float ratio = 1, bool ani = false);
    void scrollDown(float ratio = 1, bool ani = false);
    void scrollTop(bool ani = false);
    void scrollBottom(bool ani = false);
    void scrollToView(GObject* obj, bool ani = false, bool setFirst = false);
    void scrollToView(const Rect& rect, bool ani = false, bool setFirst = false);
    bool isChildInView(GObject* obj) const;

    int getPageX() const;
    void setPageX(int value, bool ani = false);
    int getPageY() const;
    void setPageY(int value, bool ani = false);

    float getScrollingPosX() const;
    float getScrollingPosY() const;

    const Size& getContentSize() const { return _contentSize; }
    const Size& getViewSize() const { return _viewSize; }

    void lockHeader(int size);
    void lockFooter(int size);

    void cancelDragging();
    static ScrollPane* getDraggingPane() { return _draggingPane; }

private:

    void onOwnerSizeChanged();
    void adjustMaskContainer();
    void setContentSize(float wv, float hv);
    void changeContentSizeOnScrolling(float deltaWidth, float deltaHeight, float deltaPosX, float deltaPosY);
    void setViewWidth(float value);
    void setViewHeight(float value);
    void setSize(float wv, float hv);
    void handleSizeChanged();

    void handleControllerChanged(GController* c);
    void updatePageController();

    GObject* hitTest(const Vec2 &pt);

    void posChanged(bool ani);
    CALL_LATER_FUNC(ScrollPane, refresh);
    void refresh2();

    void syncScrollBar(bool end = false);
    void showScrollBar(bool show);
    CALL_LATER_FUNC(ScrollPane, onShowScrollBar);

    float getLoopPartSize(float division, int axis);
    bool loopCheckingCurrent();
    void loopCheckingTarget(Vec2& endPos);
    void loopCheckingTarget(Vec2& endPos, int axis);
    void loopCheckingNewPos(float& value, int axis);
    void alignPosition(Vec2& pos, bool inertialScrolling);
    float alignByPage(float pos, int axis, bool inertialScrolling);
    Vec2 updateTargetAndDuration(const Vec2& orignPos);
    float updateTargetAndDuration(float pos, int axis);
    void fixDuration(int axis, float oldChange);
    void killTween();
    void tweenUpdate(float dt);
    float runTween(int axis, float dt);

    void checkRefreshBar();

    void onTouchBegin(EventContext* context);
    void onTouchMove(EventContext* context);
    void onTouchEnd(EventContext* context);
    void onMouseWheel(EventContext* context);
    void onRollOver(EventContext* context);
    void onRollOut(EventContext* context);

    ScrollType _scrollType;
    float _scrollStep;
    float _mouseWheelStep;
    Margin _scrollBarMargin;
    bool _bouncebackEffect;
    bool _touchEffect;
    bool _scrollBarDisplayAuto;
    bool _vScrollNone;
    bool _hScrollNone;
    bool _needRefresh;
    int _refreshBarAxis;
    bool _displayOnLeft;
    bool _snapToItem;
    bool _displayInDemand;
    bool _mouseWheelEnabled;
    bool _inertiaDisabled;
    bool _maskDisabled;
    float _decelerationRate;
    bool _pageMode;

    float _xPos;
    float _yPos;

    Size _viewSize;
    Size _contentSize;
    Size _overlapSize;
    Size _pageSize;

    Vec2 _containerPos;
    Vec2 _beginTouchPos;
    Vec2 _lastTouchPos;
    Vec2 _lastTouchGlobalPos;
    Vec2 _velocity;
    float _velocityScale;
    clock_t _lastMoveTime;
    bool _isMouseMoved;
    bool _isHoldAreaDone;
    int _aniFlag;
    bool _scrollBarVisible;
    int _loop;

    int _headerLockedSize;
    int _footerLockedSize;

    int _tweening;
    Vec2 _tweenStart;
    Vec2 _tweenChange;
    Vec2 _tweenTime;
    Vec2 _tweenDuration;

    GComponent* _owner;
    GComponent* _maskContainer;
    GScrollBar* _hzScrollBar;
    GScrollBar* _vtScrollBar;
    GComponent* _header;
    GComponent* _footer;
    GController* _pageController;

    float _scrollX;
    float _scrollY;

    static int _gestureFlag;
    static ScrollPane* _draggingPane;

    friend class GComponent;
    friend class GList;
};

NS_FGUI_END

#endif
