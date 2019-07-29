#ifndef __GOBJECT_H__
#define __GOBJECT_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "event/UIEventDispatcher.h"
#include "PackageItem.h"
#include "UIConfig.h"
#include "Controller.h"
#include "Relations.h"
#include "gears/GearBase.h"
#include "gears/GearDisplay.h"
#include "gears/GearColor.h"
#include "gears/GearAnimation.h"
#include <AzCore/Math/Matrix3x3.h>
#include <AzCore/Math/Transform.h>
#include "FieldTypes.h"

NS_FGUI_BEGIN

class GComponent;
class GGroup;
class GButton;
class GLabel;
class GComboBox;
class GProgressBar;
class GSlider;
class GList;
class GRoot;
class GTextFieldDelegate;
class GRichTextField;
class GTextInput;
class WeakPtr;
class DisplayObject;

class GObject : public UIEventDispatcher
{
    FUI_RTTI_DERIVED(GObject, UIEventDispatcher)
    
public:
    static GObject* getDraggingObject() { return _draggingObject; }

    GObject();
    virtual ~GObject();

    CREATE_FUNC(GObject);

    virtual IColorGear* GetIColorGear() { return nullptr; }
    virtual IAnimationGear* GetIAnimationGear() { return nullptr; }

    float getX() const { return _position.x; };
    void setX(float value);
    float getY() const { return _position.y; };
    void setY(float value);
    const Vec2& getPosition()const { return _position; };
    void setPosition(float xv, float yv, bool ignoreX = false, bool ignoreY = false);

    bool getPixelSnapping() const { return _pixelSnapping; }
    void setPixelSnapping(bool value);

    float getWidth() const { return _size.width; }
    void setWidth(float value) { setSize(value, _rawSize.height); }
    float getHeight() const { return _size.height; }
    void setHeight(float value) { setSize(_rawSize.width, value); }
    const Size& getSize() const { return _size; }
    void setSize(float wv, float hv, bool ignorePivot = false);
    void setSize(Size size) { _size = size; }

    void center(bool restraint = false);
    void makeFullScreen();

    const Vec2& getPivot() const { return _pivot; }
    void setPivot(float xv, float yv, bool asAnchor = false);

    float getScaleX() const { return _scale.x; }
    void setScaleX(float value) { setScale(value, _scale.y); }

    float getScaleY() const { return _scale.y; }
    void setScaleY(float value) { setScale(_scale.x, value); }

    const Vec2& getScale() const { return _scale; }
    void setScale(float xv, float yv);

    float getSkewX() const { return _skew.x; }
    void setSkewX(float value) { setSkew(value, _skew.y); }

    float getSkewY() const { return _skew.y; }
    void setSkewY(float value) { setSkew(_skew.x, value); }

    const Vec2& getSkew() const { return _skew; }
    void setSkew(float xv, float xy);

    float getRotation() const { return _rotation; }
    void setRotation(float value);

    float getAlpha() const { return _alpha; }
    void setAlpha(float value);

    bool isGrayed() const { return _finalGrayed || _grayed; }
    void setGrayed(bool value);

    bool isVisible() const { return _visible; }
    void setVisible(bool value);

    bool isTouchable() const { return _touchable; }
    void setTouchable(bool value);

    int getSortingOrder() const { return _sortingOrder; }
    void setSortingOrder(int value);

    GGroup* getGroup() const { return _group; }
    void setGroup(GGroup* value);

    virtual const AZStd::string& getText() const;
    virtual void setText(const AZStd::string& text);

    virtual const AZStd::string& getIcon() const;
    virtual void setIcon(const AZStd::string& text);

    const AZStd::string& getTooltips() const { return _tooltips; }
    void setTooltips(const AZStd::string& value);

    void* getData() const { return _data; };
    void setData(void* value) { _data = value; }

    bool isDraggable() const { return _draggable; }
    void setDraggable(bool value);
    Rect* getDragBounds() const { return _dragBounds; }
    void setDragBounds(const Rect& value);

    void startDrag(int touchId = -1);
    void stopDrag();

    AZStd::string getResourceURL() const;

    PackageItem* getPackageItem()const { return _packageItem; }

    Vec2 globalToLocal(const Vec2& pt);
    Rect globalToLocal(const Rect& rect);
    virtual Vec2 localToGlobal(const Vec2& pt);
    Rect localToGlobal(const Rect& rect);
    Rect transformRect(const Rect& rect, GObject* targetSpace);

    Vec2 convertToWorldSpace(const Vec2 & pt);
    Vec2 convertToNodeSpace(const Vec2 & pt);   
    

    Relations* relations() { return _relations; }
    void addRelation(GObject* target, RelationType relationType, bool usePercent = false);
    void removeRelation(GObject* target, RelationType relationType);

    GearBase* getGear(int index);
    bool checkGearController(int index, GController* c);
    uint32_t addDisplayLock();
    void releaseDisplayLock(uint32_t token);

    GComponent* getParent() const { return _parent; }
    void removeFromParent();

    GRoot* getRoot() const;

    void addClickListener(const EventCallback& callback) { addEventListener(UIEventType::Click, callback); }
    void addClickListener(const EventCallback& callback, const EventTag& tag) { addEventListener(UIEventType::Click, callback, tag); }
    void removeClickListener(const EventTag& tag) { removeEventListener(UIEventType::Click, tag); }

    virtual void constructFromResource();
    virtual GObject* hitTest(const Vec2 & worldPoint);

    template<typename T> T* as();

    virtual void RefreshTransform();
    AZ::Transform* GetTransform();
    virtual AZ::Transform GetLocalMatrix44();
    virtual AZ::Transform GetGlobalMatrix44();

    AZStd::string id;
    AZStd::string name;
    Size sourceSize;
    Size initSize;
    Size minSize;
    Size maxSize;

    //internal use
    bool _underConstruct;
    bool _gearLocked;

    Color4B _color4b;
    DisplayObject* GetDisplayObject() { return m_displayObject; }

protected:
    GComponent* _parent;
    PackageItem* _packageItem;
    int _sizeImplType;
    bool _touchDisabled;

    virtual void handleInit();
    virtual void handleSizeChanged();
    virtual void handleScaleChanged();
    virtual void handleSkewChanged(){}
    virtual void handleGrayedChanged();
    virtual void handlePositionChanged();
    virtual void handleControllerChanged(GController* c);
    virtual void handleAlphaChanged();
    virtual void handleVisibleChanged();

    virtual void onEnter();
    virtual void onExit();

    virtual void setup_BeforeAdd(TXMLElement* xml);
    virtual void setup_AfterAdd(TXMLElement* xml);

    bool init();

    void updateGear(int index);
    void checkGearDisplay();

    void setSizeDirectly(float wv, float hv);

    Vec2 _position;
    Size _size;
    Size _rawSize;
    Vec2 _pivot;
    Vec2 _scale;
    Vec2 _skew;
    bool _pivotAsAnchor;
    float _alpha;
    float _rotation;
    bool _visible;
    bool _touchable;
    bool _grayed;
    bool _finalGrayed;


    AZ::Transform _transform;
    DisplayObject* m_displayObject = nullptr;
//    Transform _transform;
private:
    void RefreshSelfTransform();

    bool internalVisible() const;
    bool internalVisible2() const;
    void updateGearFromRelations(int index, float dx, float dy);
    void transformRectPoint(const Vec2& pt, float rect[], GObject* targetSpace);

    void initDrag();
    void dragBegin(int touchId);
    void dragEnd();
    void onTouchBegin(EventContext* context);
    void onTouchMove(EventContext* context);
    void onTouchEnd(EventContext* context);
    void onRollOver(EventContext* context);
    void onRollOut(EventContext* context);

    bool _internalVisible;
    bool _handlingController;
    bool _draggable;
    int _sortingOrder;
    bool _focusable;
    AZStd::string _tooltips;
    bool _pixelSnapping;
    GGroup* _group;
    float _sizePercentInGroup;
    Relations* _relations;
    GearBase* _gears[8];
    void * _data;
    Vec2 _dragTouchStartPos;
    Rect* _dragBounds;
    bool _dragTesting;
    bool _isAdoptiveChild;

    uint64_t _uid;
    size_t _weakPtrRef;

    static GObject* _draggingObject;

    friend class GComponent;
    friend class GGroup;
    friend class RelationItem;
    friend class UIObjectFactory;
    friend class WeakPtr;
};

template<typename T>
inline T* GObject::as()
{
    return this->SafeCast<T>();
}
NS_FGUI_END

#endif
