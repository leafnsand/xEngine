#include "GRoot.h"
#include "UIPackage.h"
#include "render/RenderDataProcess.h"
//#include "AudioEngine.h"

NS_FGUI_BEGIN


GRoot* GRoot::_inst = nullptr;
bool GRoot::_soundEnabled = true;
float GRoot::_soundVolumeScale = 1.0f;

//GRoot* GRoot::create(Scene* scene, int zOrder)
//{
//    GRoot *pRet = newGRoot();
//    if (pRet && pRet->initWithScene(scene, zOrder))
//    {
//        pRet->autorelease();
//        return pRet;
//    }
//    else
//    {
//        delete pRet;
//        pRet = nullptr;
//        return nullptr;
//    }
//}

GRoot* GRoot::create(int zOrder)
{
    GRoot *pRet = new GRoot();
    if (pRet && pRet->initWithScene(zOrder))
    {
        _inst = pRet;
        pRet->onEnter();
        pRet->retain();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}
void GRoot::destroy()
{
    if(UIRoot)
    {
        UIRoot->onExit();
        delete _inst;
        _inst = nullptr;
    }
}

GRoot::GRoot() :
    //_windowSizeListener(nullptr),
    _inputProcessor(nullptr),
    _modalLayer(nullptr),
    _modalWaitPane(nullptr),
    _tooltipWin(nullptr),
    _defaultTooltipWin(nullptr),
    _currentFocus(nullptr),
    _showKeyBoardFunc(nullptr),
    _hideKeyBoardFunc(nullptr)
{
}

GRoot::~GRoot()
{
    delete _inputProcessor;
    FUI_SAFE_RELEASE(_modalWaitPane);
    FUI_SAFE_RELEASE(_defaultTooltipWin);
    FUI_SAFE_RELEASE(_modalLayer);
    CALL_LATER_CANCEL(GRoot, doShowTooltipsWin);

//    if (_windowSizeListener)
//        Director::getInstance()->getEventDispatcher()->removeEventListener(_windowSizeListener);
}

void GRoot::showWindow(Window * win)
{
    addChild(win);
    adjustModalLayer();
}

void GRoot::hideWindow(Window * win)
{
    win->hide();
}

void GRoot::hideWindowImmediately(Window * win)
{
    if (win->getParent() == this)
        removeChild(win);

    adjustModalLayer();
}

void GRoot::bringToFront(Window * win)
{
    int cnt = numChildren();
    int i;
    if (_modalLayer->getParent() != nullptr && !win->isModal())
        i = getChildIndex(_modalLayer) - 1;
    else
        i = cnt - 1;

    for (; i >= 0; i--)
    {
        GObject* g = getChildAt(i);
        if (g == win)
            return;
        if (g->SafeCast<Window>())
            break;
    }

    if (i >= 0)
        setChildIndex(win, i);
}

void GRoot::closeAllExceptModals()
{
    Vector<GObject*> map(_children);

    for (const auto&child : map)
    {
        if (child->SafeCast<Window>() && !((Window*)child)->isModal())
            hideWindowImmediately((Window*)child);
    }
}

void GRoot::closeAllWindows()
{
    Vector<GObject*> map(_children);

    for (const auto&child : map)
    {
        if (child->SafeCast<Window>())
            hideWindowImmediately((Window*)child);
    }
}

Window * GRoot::getTopWindow()
{
    int cnt = numChildren();
    for (int i = cnt - 1; i >= 0; i--)
    {
        GObject* child = getChildAt(i);
        if (child->SafeCast<Window>())
        {
            return (Window*)child;
        }
    }

    return nullptr;
}

GGraph * GRoot::getModalLayer()
{
    if (_modalLayer == nullptr)
        createModalLayer();

    return _modalLayer;
}

void GRoot::createModalLayer()
{
    _modalLayer = GGraph::create();
    _modalLayer->retain();
    _modalLayer->drawRect(getWidth(), getHeight(), 0, Color4F::WHITE, UIConfig::getInstance()->modalLayerColor);
    _modalLayer->addRelation(this, RelationType::Size);
}

void GRoot::adjustModalLayer()
{
    if (_modalLayer == nullptr)
        createModalLayer();

    int cnt = numChildren();

    if (_modalWaitPane != nullptr && _modalWaitPane->getParent() != nullptr)
        setChildIndex(_modalWaitPane, cnt - 1);

    for (int i = cnt - 1; i >= 0; i--)
    {
        GObject* child = getChildAt(i);
        if (child->SafeCast<Window>() && ((Window*)child)->isModal())
        {
            if (_modalLayer->getParent() == nullptr)
                addChildAt(_modalLayer, i);
            else
                setChildIndexBefore(_modalLayer, i);
            return;
        }
    }

    if (_modalLayer->getParent() != nullptr)
        removeChild(_modalLayer);
}

bool GRoot::hasModalWindow()
{
    return _modalLayer != nullptr && _modalLayer->getParent() != nullptr;
}

void GRoot::showModalWait()
{
    getModalWaitingPane();
    if (_modalWaitPane)
        addChild(_modalWaitPane);
}

void GRoot::closeModalWait()
{
    if (_modalWaitPane != nullptr && _modalWaitPane->getParent() != nullptr)
        removeChild(_modalWaitPane);
}

GObject * GRoot::getModalWaitingPane()
{
    if (!UIConfig::getInstance()->globalModalWaiting.empty())
    {
        if (_modalWaitPane == nullptr)
        {
            _modalWaitPane = UIPackage::createObjectFromURL(UIConfig::getInstance()->globalModalWaiting);
            _modalWaitPane->setSortingOrder(INT_MAX);
            _modalWaitPane->retain();
        }

        _modalWaitPane->setSize(getWidth(), getHeight());
        _modalWaitPane->addRelation(this, RelationType::Size);

        return _modalWaitPane;
    }
    else
        return nullptr;
}

bool GRoot::isModalWaiting()
{
    return (_modalWaitPane != nullptr);// && _modalWaitPane->onStage();
}

Vec2 GRoot::getTouchPosition(int touchId)
{
    return _inputProcessor->getTouchPosition(touchId);
}

GObject * GRoot::getTouchTarget()
{
    return _inputProcessor->getRecentInput()->getTarget();
}

void GRoot::showPopup(GObject * popup)
{
    showPopup(popup, nullptr, PopupDirection::AUTO);
}

void GRoot::showPopup(GObject * popup, GObject * target, PopupDirection dir)
{
    if (!_popupStack.empty())
        hidePopup(popup);

    _popupStack.push_back(WeakPtr(popup));

    addChild(popup);
    adjustModalLayer();

    if (popup->SafeCast<Window>() && target == nullptr && dir == PopupDirection::AUTO)
        return;

    Vec2 pos = getPoupPosition(popup, target, dir);
    popup->setPosition(pos.x, pos.y);
}

void GRoot::togglePopup(GObject * popup)
{
    togglePopup(popup, nullptr, PopupDirection::AUTO);
}

void GRoot::togglePopup(GObject * popup, GObject * target, PopupDirection dir)
{
    if (find(_justClosedPopups.cbegin(), _justClosedPopups.cend(), popup) != _justClosedPopups.cend())
        return;

    showPopup(popup, target, dir);
}

void GRoot::hidePopup()
{
    hidePopup(nullptr);
}

void GRoot::hidePopup(GObject * popup)
{
    if (popup != nullptr)
    {
        auto it = find(_popupStack.cbegin(), _popupStack.cend(), popup);
        if (it != _popupStack.cend())
        {
            int k = (int)(it - _popupStack.cbegin());
            for (int i = (int)_popupStack.size() - 1; i >= k; i--)
            {
                closePopup(_popupStack.back().ptr());
                _popupStack.pop_back();
            }
        }
    }
    else
    {
        for (const auto &it : _popupStack)
            closePopup(it.ptr());
        _popupStack.clear();
    }
}

void GRoot::closePopup(GObject * target)
{
    if (target && target->getParent() != nullptr)
    {
        if (target->SafeCast<Window>())
            ((Window*)target)->hide();
        else
            removeChild(target);
    }
}

void GRoot::checkPopups()
{
    _justClosedPopups.clear();
    if (!_popupStack.empty())
    {
        GObject* mc = _inputProcessor->getRecentInput()->getTarget();
        bool handled = false;
        while (mc != this && mc != nullptr)
        {
            auto it = find(_popupStack.cbegin(), _popupStack.cend(), mc);
            if (it != _popupStack.cend())
            {
                int k = (int)(it - _popupStack.cbegin());
                for (int i = (int)_popupStack.size() - 1; i > k; i--)
                {
                    closePopup(_popupStack.back().ptr());
                    _popupStack.pop_back();
                }
                handled = true;
                break;
            }
            mc = mc->getParent();
        }

        if (!handled)
        {
            for (int i = (int)_popupStack.size() - 1; i >= 0; i--)
            {
                GObject* popup = _popupStack[i].ptr();
                if (popup)
                {
                    _justClosedPopups.push_back(WeakPtr(popup));
                    closePopup(popup);
                }
            }
            _popupStack.clear();
        }
    }
}

bool GRoot::hasAnyPopup()
{
    return !_popupStack.empty();
}

Vec2 GRoot::getPoupPosition(GObject * popup, GObject * target, PopupDirection dir)
{
    Vec2 pos;
    Vec2 size;
    if (target != nullptr)
    {
        pos = target->localToGlobal(Vec2::ZERO);
        pos = this->globalToLocal(pos);
        size = target->localToGlobal(target->getSize());
        size = this->globalToLocal(size);
        size -= pos;
    }
    else
    {
        pos = globalToLocal(_inputProcessor->getRecentInput()->getPosition());
    }
    float xx, yy;
    xx = pos.x;
    if (xx + popup->getWidth() > RenderDataProcess::DefaultWidth)
        xx = xx + size.x - popup->getWidth();
    yy = pos.y + size.y;
    if ((dir == PopupDirection::AUTO && yy + popup->getHeight() > RenderDataProcess::DefaultHeight)
        || dir == PopupDirection::UP)
    {
        yy = pos.y - popup->getHeight() - 1;
        if (yy < 0)
        {
            yy = 0;
            xx += size.x / 2;
        }
    }

    return Vec2(round(xx), round(yy));
}

void GRoot::showTooltips(const AZStd::string & msg)
{
    if (_defaultTooltipWin == nullptr)
    {
        const AZStd::string& resourceURL = UIConfig::getInstance()->tooltipsWin;
        if (resourceURL.empty())
        {
            f_printf("FairyGUI: UIConfig.tooltipsWin not defined");
            return;
        }

        _defaultTooltipWin = UIPackage::createObjectFromURL(resourceURL);
        _defaultTooltipWin->setTouchable(false);
        _defaultTooltipWin->retain();
    }

    _defaultTooltipWin->setText(msg);
    showTooltipsWin(_defaultTooltipWin);
}

void GRoot::showTooltipsWin(GObject * tooltipWin)
{
    hideTooltips();

    _tooltipWin = tooltipWin;
    CALL_LATER(GRoot, doShowTooltipsWin, 0.1f);
}

void GRoot::doShowTooltipsWin()
{
    if (_tooltipWin == nullptr)
        return;

    Vec2 pt = _inputProcessor->getRecentInput()->getPosition();
    float xx = pt.x + 10;
    float yy = pt.y + 20;

    pt = globalToLocal(Vec2(xx, yy));
    xx = pt.x;
    yy = pt.y;

    if (xx + _tooltipWin->getWidth() > getWidth())
        xx = xx - _tooltipWin->getWidth();
    if (yy + _tooltipWin->getHeight() > getHeight())
    {
        yy = yy - _tooltipWin->getHeight() - 1;
        if (yy < 0)
            yy = 0;
    }

    _tooltipWin->setPosition(round(xx), round(yy));
    addChild(_tooltipWin);
}

void GRoot::hideTooltips()
{
    if (_tooltipWin != nullptr)
    {
        if (_tooltipWin->getParent() != nullptr)
            removeChild(_tooltipWin);
        _tooltipWin = nullptr;
    }
}

void GRoot::playSound(const AZStd::string & url, float volumnScale)
{
    if (!_soundEnabled)
        return;

    PackageItem* pi = UIPackage::getItemByURL(url);
//    if (pi)
//        experimental::AudioEngine::play2d(pi->file, false, _soundVolumeScale * volumnScale);
}

void GRoot::setSoundEnabled(bool value)
{
    _soundEnabled = value;
}

void GRoot::setSoundVolumeScale(float value)
{
    _soundVolumeScale = value;
}

void GRoot::onTouchEvent(int eventType)
{
    if (eventType == UIEventType::TouchBegin)
    {
        if (_tooltipWin != nullptr)
            hideTooltips();

        checkPopups();
        checkFocusChanged();
    }
}

void GRoot::handlePositionChanged()
{
    //_displayObject->setPosition(0, _size.height);
}


void GRoot::onEnter()
{
    GComponent::onEnter();
}

void GRoot::onExit()
{
    GComponent::onExit();
}

bool GRoot::initWithScene(int zOrder)
{
    if (!GComponent::init())
        return false;

    _inputProcessor = new InputProcessor(this);
    _inputProcessor->setCaptureCallback(FUI_CALLBACK_1(GRoot::onTouchEvent, this));

#ifdef CC_PLATFORM_PC
    _windowSizeListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(GLViewImpl::EVENT_WINDOW_RESIZED, FUI_CALLBACK_0(GRoot::onWindowSizeChanged, this));
#endif
    onWindowSizeChanged();

    //scene->addChild(_displayObject, zOrder);
    
    addEventListener(UIEventType::KeyDown, FUI_CALLBACK_1(GRoot::onKeyDown, this));
    addEventListener(UIEventType::KeyUp, FUI_CALLBACK_1(GRoot::onKeyUp, this));

    return true;
}

void GRoot::onKeyDown(EventContext * context)
{
    if (_currentFocus && _currentFocus != this)
    {
        _currentFocus->dispatchEvent(UIEventType::KeyDown);
    }
}

void GRoot::onKeyUp(EventContext * context)
{
    if (_currentFocus && _currentFocus != this)
    {
        _currentFocus->dispatchEvent(UIEventType::KeyUp);
    }
}
//bool GRoot::initWithScene(Scene * scene, int zOrder)
//{
//    if (!GComponent::init())
//        return false;
//
//    _inputProcessor = new InputProcessor(this);
//    _inputProcessor->setCaptureCallback(FUI_CALLBACK_1(GRoot::onTouchEvent, this));
//
//#ifdef CC_PLATFORM_PC
//    _windowSizeListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(GLViewImpl::EVENT_WINDOW_RESIZED, FUI_CALLBACK_0(GRoot::onWindowSizeChanged, this));
//#endif
//    onWindowSizeChanged();
//
//    scene->addChild(_displayObject, zOrder);
//
//    return true;
//}

void GRoot::onWindowSizeChanged()
{
//    const Size& rs = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
//    setSize(rs.width, rs.height);
}

void GRoot::setKeyBoardFunc(ShowKeyBoardCallBack showFunc, HideKeyBoardCallBack hideFunc)
{
    _showKeyBoardFunc = showFunc;
    _hideKeyBoardFunc = hideFunc;
}

void GRoot::checkFocusChanged()
{
    GObject* focus = _inputProcessor->getRecentInput()->getTarget();
    if(_currentFocus != nullptr && _currentFocus != focus)
    {
        GTextInput* input = _currentFocus->SafeCast<GTextInput>();
        if (input)
        {
            input->OnFocusOut();
            if (_hideKeyBoardFunc) {
                _hideKeyBoardFunc(input);
            }
        }
        _currentFocus->release();
    }

    _currentFocus = focus;

    if(_currentFocus != nullptr)
    {
        _currentFocus->retain();
        GTextInput* input = _currentFocus->SafeCast<GTextInput>();
        if (input)
        {
            input->OnFocusIn();
            if (_showKeyBoardFunc) {
                _showKeyBoardFunc(input);
            }
        }
    }
}
NS_FGUI_END