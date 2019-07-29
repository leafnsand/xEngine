#include "GComboBox.h"
#include "utils/ToolSet.h"
#include "GRoot.h"
#include "GButton.h"
#include "UIPackage.h"
#include "GTextInput.h"

NS_FGUI_BEGIN


GComboBox::GComboBox() :
    _dropdown(nullptr),
    _titleObject(nullptr),
    _iconObject(nullptr),
    _list(nullptr),
    _selectionController(nullptr),
    _itemsUpdated(true),
    _selectedIndex(-1),
    popupDirection(PopupDirection::AUTO)
{
    visibleItemCount = UIConfig::getInstance()->defaultComboBoxVisibleItemCount;
}

GComboBox::~GComboBox()
{
    FUI_SAFE_RELEASE(_dropdown);
}

const AZStd::string& GComboBox::getTitle() const
{
    if (_titleObject != nullptr)
        return _titleObject->getText();
    else
        return UIConfig::getInstance()->emptyString;
}

void GComboBox::setTitle(const AZStd::string & value)
{
    if (_titleObject != nullptr)
        _titleObject->setText(value);
    updateGear(6);
}

const AZStd::string& GComboBox::getIcon() const
{
    if (_iconObject != nullptr)
        return _iconObject->getIcon();
    else
        return UIConfig::getInstance()->emptyString;
}

void GComboBox::setIcon(const AZStd::string & value)
{
    if (_iconObject != nullptr)
        _iconObject->setIcon(value);
    updateGear(7);
}

const AZStd::string & GComboBox::getValue() const
{
    if (_selectedIndex >= 0 && _selectedIndex < (int)_values.size())
        return _values[_selectedIndex];
    else
        return UIConfig::getInstance()->emptyString;
}

void GComboBox::setValue(const AZStd::string & value)
{
    setSelectedIndex(ToolSet::findInStringArray(_values, value));
}

void GComboBox::setSelectedIndex(int value)
{
    if (_selectedIndex == value)
        return;

    _selectedIndex = value;
    if (_selectedIndex >= 0 && _selectedIndex < (int)_items.size())
    {
        setText(_items[_selectedIndex]);
        if (!_icons.empty() && _selectedIndex != -1 && _selectedIndex < (int)_icons.size())
            setIcon(_icons[_selectedIndex]);
    }
    else
    {
        setTitle(UIConfig::getInstance()->emptyString);
        if (!_icons.empty())
            setIcon(UIConfig::getInstance()->emptyString);
    }

    updateSelectionController();
}

void GComboBox::refresh()
{
    if (!_items.empty())
    {
        if (_selectedIndex >= (int)_items.size())
            _selectedIndex = (int)_items.size() - 1;
        else if (_selectedIndex == -1)
            _selectedIndex = 0;
        setTitle(_items[_selectedIndex]);
    }
    else
    {
        setTitle(UIConfig::getInstance()->emptyString);
        _selectedIndex = -1;
    }

    if (!_icons.empty())
    {
        if (_selectedIndex != -1 && _selectedIndex < (int)_icons.size())
            setIcon(_icons[_selectedIndex]);
        else
            setIcon(UIConfig::getInstance()->emptyString);
    }

    _itemsUpdated = true;
}

void GComboBox::setState(const AZStd::string& value)
{
    if (_buttonController != nullptr)
        _buttonController->setSelectedPage(value);
}

void GComboBox::setCurrentState()
{
    if (isGrayed() && _buttonController != nullptr && _buttonController->hasPage(GButton::DISABLED))
        setState(GButton::DISABLED);
    else if (_dropdown != nullptr && _dropdown->getParent() != nullptr)
        setState(GButton::DOWN);
    else
        setState(_over ? GButton::OVER : GButton::UP);
}

void GComboBox::updateSelectionController()
{
    if (_selectionController != nullptr && !_selectionController->changing
        && _selectedIndex < _selectionController->getPageCount())
    {
        GController* c = _selectionController;
        _selectionController = nullptr;
        c->setSelectedIndex(_selectedIndex);
        _selectionController = c;
    }
}

void GComboBox::updateDropdownList()
{
    if (_itemsUpdated)
    {
        _itemsUpdated = false;
        renderDropdownList();
        _list->resizeToFit(visibleItemCount);
    }
}

void GComboBox::showDropdown()
{
    updateDropdownList();
    if (_list->getSelectionMode() == ListSelectionMode::SINGLE)
        _list->setSelectedIndex(-1);
    _dropdown->setWidth(_size.width);

    UIRoot->togglePopup(_dropdown, this, popupDirection);
    if (_dropdown->getParent() != nullptr)
        setState(GButton::DOWN);
}

void GComboBox::renderDropdownList()
{
    _list->removeChildrenToPool();
    size_t cnt = _items.size();
    for (size_t i = 0; i < cnt; i++)
    {
        GObject* item = _list->addItemFromPool();
        item->setText(_items[i]);
        item->setIcon((!_icons.empty() && i < _icons.size()) ? _icons[i] : UIConfig::getInstance()->emptyString);
        item->name = i < _values.size() ? _values[i] : UIConfig::getInstance()->emptyString;
    }
}

void GComboBox::handleControllerChanged(GController* c)
{
    GComponent::handleControllerChanged(c);

    if (_selectionController == c)
        setSelectedIndex(c->getSelectedIndex());
}

void GComboBox::handleGrayedChanged()
{
    if (_buttonController != nullptr && _buttonController->hasPage(GButton::DISABLED))
    {
        if (isGrayed())
            setState(GButton::DISABLED);
        else
            setState(GButton::UP);
    }
    else
        GComponent::handleGrayedChanged();
}

void GComboBox::constructFromXML(TXMLElement * xml)
{
    GComponent::constructFromXML(xml);

    _buttonController = getController("button");
    _titleObject = getChild("title");
    _iconObject = getChild("icon");

    xml = xml->FirstChildElement("ComboBox");

    const char* p;
    p = xml->Attribute("dropdown");
    if (p)
    {
        _dropdown = UIPackage::createObjectFromURL(p)->SafeCast<GComponent>();
        f_assert2(_dropdown != nullptr, "FairyGUI: should be a component.");

        _dropdown->retain();

        _list = _dropdown->getChild("list")->SafeCast<GList>();
        f_assert2(_list != nullptr, "FairyGUI: should container a list component named list.");

        _list->addEventListener(UIEventType::ClickItem, FUI_CALLBACK_1(GComboBox::onClickItem, this));

        _list->addRelation(_dropdown, RelationType::Width);
        _list->removeRelation(_dropdown, RelationType::Height);

        _dropdown->addRelation(_list, RelationType::Height);
        _dropdown->removeRelation(_list, RelationType::Width);

        _dropdown->addEventListener(UIEventType::Exit, FUI_CALLBACK_1(GComboBox::onPopupWinClosed, this));
    }

    addEventListener(UIEventType::RollOver, FUI_CALLBACK_1(GComboBox::onRollover, this));
    addEventListener(UIEventType::RollOut, FUI_CALLBACK_1(GComboBox::onRollout, this));
    addEventListener(UIEventType::TouchBegin, FUI_CALLBACK_1(GComboBox::onTouchBegin, this));
    addEventListener(UIEventType::TouchEnd, FUI_CALLBACK_1(GComboBox::onTouchEnd, this));
}

void GComboBox::setup_AfterAdd(TXMLElement * xml)
{
    GComponent::setup_AfterAdd(xml);

    xml = xml->FirstChildElement("ComboBox");
    if (!xml)
        return;

    const char *p;

    int vc = xml->IntAttribute("visibleItemCount");
    if (vc != 0)
        visibleItemCount = vc;
    p = xml->Attribute("direction");
    if (p)
        popupDirection = ToolSet::parsePopupDirection(p);

    TXMLElement* cxml = xml->FirstChildElement("item");
    bool hasIcon = false;
    while (cxml)
    {
        p = cxml->Attribute("title");
        if (p)
            _items.push_back(p);
        else
            _items.push_back(UIConfig::getInstance()->emptyString);

        p = cxml->Attribute("value");
        if (p)
            _values.push_back(p);
        else
            _values.push_back(UIConfig::getInstance()->emptyString);

        p = cxml->Attribute("icon");
        if (p)
        {
            if (!hasIcon)
            {
                for (int i = 0; i < (int)_items.size() - 1; i++)
                    _icons.push_back(UIConfig::getInstance()->emptyString);
            }
            _icons.push_back(p);
        }
        else if (hasIcon)
            _icons.push_back(UIConfig::getInstance()->emptyString);

        cxml = cxml->NextSiblingElement("item");
    }

    p = xml->Attribute("title");
    if (p && strlen(p) > 0)
    {
        setTitle(p);
        _selectedIndex = ToolSet::findInStringArray(_items, p);
    }
    else if (!_items.empty())
    {
        _selectedIndex = 0;
        setTitle(_items[0]);
    }
    else
        _selectedIndex = -1;

    p = xml->Attribute("icon");
    if (p && strlen(p) > 0)
        this->setIcon(p);

    p = xml->Attribute("selectionController");
    if (p)
        _selectionController = _parent->getController(p);
}

void GComboBox::onClickItem(EventContext* context)
{
    if (_dropdown->getParent()->SafeCast<GRoot>())
        ((GRoot*)_dropdown->getParent())->hidePopup(_dropdown);
    _selectedIndex = INT_MIN;
    setSelectedIndex(_list->getChildIndex((GObject*)context->getData()));

    dispatchEvent(UIEventType::ClickItem);
}

void GComboBox::onRollover(EventContext* context)
{
    _over = true;
    if (_down || (_dropdown != nullptr && _dropdown->getParent() != nullptr))
        return;

    setCurrentState();
}

void GComboBox::onRollout(EventContext* context)
{
    _over = false;
    if (_down || (_dropdown != nullptr && _dropdown->getParent() != nullptr))
        return;

    setCurrentState();
}

void GComboBox::onTouchBegin(EventContext* context)
{
    if (context->getInput()->getButton() != MouseButton::BUTTON_LEFT)
        return;

    if (context->getInput()->getTarget()->SafeCast<GTextInput>())
        return;

    _down = true;

    if (_dropdown != nullptr)
        showDropdown();

    context->captureTouch();
}

void GComboBox::onTouchEnd(EventContext* context)
{
    if (context->getInput()->getButton() != MouseButton::BUTTON_LEFT)
        return;

    if (_down)
    {
        _down = false;
        if (_dropdown != nullptr && _dropdown->getParent() != nullptr)
            setCurrentState();
    }
}

void GComboBox::onPopupWinClosed(EventContext* context)
{
    setCurrentState();
}

NS_FGUI_END
