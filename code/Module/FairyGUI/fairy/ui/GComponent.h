#ifndef __GCOMPONENT_H__
#define __GCOMPONENT_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "GObject.h"
#include "Transition.h"
#include "Margin.h"
#include "ScrollPane.h"
#include "event/HitTest.h"

NS_FGUI_BEGIN

class GController;
class GGroup;
class Container;

class GComponent : public GObject
{
    FUI_RTTI_DERIVED(GComponent, GObject)
public:
    GComponent();
    virtual ~GComponent();

    CREATE_FUNC(GComponent);

    GObject* addChild(GObject* child, Container* realFatherContainer = nullptr);
    virtual GObject* addChildAt(GObject* child, int index, Container* realFatherContainer = nullptr);

    void removeChild(GObject * child);
    virtual void removeChildAt(int index);
    void removeChildren() { removeChildren(0, -1); }
    void removeChildren(int beginIndex, int endIndex);

    GObject * getChildAt(int index) const;
    GObject * getChild(const AZStd::string& name) const;
    GObject * getChildInGroup(const GGroup * group, const AZStd::string& name) const;
    GObject * getChildById(const AZStd::string& id) const;
    const Vector<GObject*>& getChildren() const { return _children; }

    int getChildIndex(const GObject* child) const;
    void setChildIndex(GObject* child, int index);
    int setChildIndexBefore(GObject* child, int index);
    void swapChildren(GObject* child1, GObject* child2);
    void swapChildrenAt(int index1, int index2);

    int numChildren() const;
    bool isAncestorOf(const GObject* obj) const;

    virtual bool isChildInView(GObject* child);
    virtual int getFirstChildInView();

    void addController(GController* c);
    GController* getControllerAt(int index) const;
    GController* getController(const AZStd::string& name) const;
    const Vector<GController*>& getControllers() const { return _controllers; }
    void removeController(GController* c);
    void applyController(GController* c);
    void applyAllControllers();

    Transition* getTransition(const AZStd::string& name) const;
    Transition* getTransitionAt(int index) const;
    const Vector<Transition*>& getTransitions() const { return _transitions; }

    bool getOpaque() const { return _opaque; }
    void setOpaque(bool value);

    const Vec2& GetAlignOffset() const { return _alignOffset; };
    const Margin& getMargin() { return _margin; }
    void setMargin(const Margin& value);

    ChildrenRenderOrder getChildrenRenderOrder() const { return _childrenRenderOrder; }
    void setChildrenRenderOrder(ChildrenRenderOrder value);
    int getApexIndex() const { return _apexIndex; }
    void setApexIndex(int value);

    GObject* getMask() const;
    void setMask(GObject* value, bool inverted = false);

    IHitTest* getHitArea() const { return _hitArea; }
    void setHitArea(IHitTest* value);

    ScrollPane* getScrollPane() const { return _scrollPane; }

    float getViewWidth() const;
    void setViewWidth(float value);
    float getViewHeight() const;
    void setViewHeight(float value);

    void setBoundsChangedFlag();
    void ensureBoundsCorrect();

    virtual GObject* hitTest(const Vec2 & worldPoint) override;
    virtual Vec2 getSnappingPosition(const Vec2& pt);

    //internal use
    void childSortingOrderChanged(GObject* child, int oldValue, int newValue);
    void childStateChanged(GObject * child);
    void adjustRadioGroupDepth(GObject* obj, GController* c);

    virtual void constructFromResource() override;
    void constructFromResource(vector<GObject*>* objectPool, int poolIndex);

    bool _buildingDisplayList;

    bool isClippingEnabled() const { return _clippingEnabled; }
    void setClippingEnabled(bool value) { _clippingEnabled = value; }
    bool isInverted() const { return _isInverted; }
    const Rect& getClippingRegion() const { return _clippingRect; }
    void setClippingRegion(const Rect& clippingRegion) { _clippingRect = clippingRegion; }


    void setPosition2(const Vec2 &position) { setPosition(position.x, _parent->getSize().height - position.y); }
    Vec2 getPosition2() { return Vec2(_position.x, _parent->getSize().height - _position.y); }
    void setPosition2(float x, float y) { setPosition(x, _parent->getSize().height - y); }
    void  setPositionY2(float y) { setY(_parent->getSize().height - y); }
    float getPositionY2(void) const { return  _parent->getSize().height - _position.y; }

    Transition* addNewTransition(const Transition& transition,const GObject& object);
    void RemoveTransition(const char* name);

protected:
    virtual void constructFromXML(TXMLElement* xml);
    virtual void setup_AfterAdd(TXMLElement* xml) override;
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void handleGrayedChanged() override;
    virtual void handleControllerChanged(GController* c) override;

    virtual void onEnter() override;
    virtual void onExit() override;

    virtual void updateBounds();
    void setBounds(float ax, float ay, float aw, float ah);

    void setupOverflow(OverflowType overflow);
    void setupScroll(const Margin& scrollBarMargin,
        ScrollType scroll, ScrollBarDisplayType scrollBarDisplay, int flags,
        const AZStd::string& vtScrollBarRes, const AZStd::string& hzScrollBarRes,
        const AZStd::string& headerRes, const AZStd::string& footerRes);

    Vector<GObject*> _children;
    Vector<GController*> _controllers;
    Vector<Transition*> _transitions;
    ScrollPane* _scrollPane;
    Margin _margin;
    Vec2 _alignOffset;
    ChildrenRenderOrder _childrenRenderOrder;
    int _apexIndex;
    bool _boundsChanged;
    bool _trackBounds;
    GObject* _maskOwner;
    IHitTest* _hitArea;

private:
    int getInsertPosForSortingChild(GObject * target);
    int moveChild(GObject* child, int oldIndex, int index);

    void addAdoptiveChild(GObject* child);

    CALL_LATER_FUNC(GComponent, doUpdateBounds);
    CALL_LATER_FUNC(GComponent, buildNativeDisplayList);

    bool _opaque;
    int _sortingChildCount;
    GController* _applyingController;

    friend class ScrollPane;

    bool _clippingEnabled;
    bool _isInverted;
    Rect _clippingRect;
};

NS_FGUI_END

#endif
