#include "GButton.h"
#include "utils/ToolSet.h"
#include "GLabel.h"
#include "GTextField.h"
#include "GRoot.h"

NS_FGUI_BEGIN


const char* GButton::UP = "up";
const char* GButton::DOWN = "down";
const char* GButton::OVER = "over";
const char* GButton::SELECTED_OVER = "selectedOver";
const char* GButton::DISABLED = "disabled";
const char* GButton::SELECTED_DISABLED = "selectedDisabled";

GButton::GButton() :
    _mode(ButtonMode::COMMON),
    _titleObject(nullptr),
    _iconObject(nullptr),
    _buttonController(nullptr),
    _relatedController(nullptr),
    _selected(false),
    _over(false),
    _down(false),
    _downEffect(0),
    _downScaled(false),
    _downGray(false),
    _downEffectValue(0.8f),
    _changeStateOnClick(true)
{
    _sound = UIConfig::getInstance()->buttonSound;
    _soundVolumeScale = UIConfig::getInstance()->buttonSoundVolumeScale;
}

GButton::~GButton()
{

}

void GButton::setTitle(const AZStd::string & value)
{
    _title = value;
    if (_titleObject != nullptr)
        _titleObject->setText((_selected && _selectedTitle.length() > 0) ? _selectedTitle : _title);
    updateGear(6);
}

void GButton::setIcon(const AZStd::string & value)
{
    _icon = value;
    if (_iconObject != nullptr)
        _iconObject->setIcon((_selected && _selectedIcon.length() > 0) ? _selectedIcon : _icon);
    updateGear(7);
}

void GButton::setSelectedTitle(const AZStd::string & value)
{
    _selectedTitle = value;
    if (_titleObject != nullptr)
        _titleObject->setText((_selected && _selectedTitle.length() > 0) ? _selectedTitle : _title);
}

void GButton::setSelectedIcon(const AZStd::string & value)
{
    _selectedIcon = value;
    if (_iconObject != nullptr)
        _iconObject->setIcon((_selected && _selectedIcon.length() > 0) ? _selectedIcon : _icon);
}

const Color4B& GButton::getTitleColor() const
{
    if (_titleObject->SafeCast<GTextField>())
        return ((GTextField*)_titleObject)->getColor();
    else if (_titleObject->SafeCast<GLabel>())
        return ((GLabel*)_titleObject)->getTitleColor();
    else if (_titleObject->SafeCast<GButton>())
        return ((GButton*)_titleObject)->getTitleColor();
    else
        return Color4B::BLACK;
}

void GButton::setTitleColor(const Color4B & value)
{
    if (_titleObject->SafeCast<GTextField>())
        ((GTextField*)_titleObject)->setColor(value);
    else if (_titleObject->SafeCast<GLabel>())
        ((GLabel*)_titleObject)->setTitleColor(value);
    else if (_titleObject->SafeCast<GButton>())
        ((GButton*)_titleObject)->setTitleColor(value);
}

int GButton::getTitleFontSize() const
{
    if (_titleObject->SafeCast<GTextField>())
        return ((GTextField*)_titleObject)->getFontSize();
    else if (_titleObject->SafeCast<GLabel>())
        return ((GLabel*)_titleObject)->getTitleFontSize();
    else if (_titleObject->SafeCast<GButton>())
        return ((GButton*)_titleObject)->getTitleFontSize();
    else
        return 0;
}

void GButton::setTitleFontSize(int value)
{
    if (_titleObject->SafeCast<GTextField>())
        ((GTextField*)_titleObject)->setFontSize(value);
    else if (_titleObject->SafeCast<GLabel>())
        ((GLabel*)_titleObject)->setTitleFontSize(value);
    else if (_titleObject->SafeCast<GButton>())
        ((GButton*)_titleObject)->setTitleFontSize(value);
}

void GButton::setSelected(bool value)
{
    if (_mode == ButtonMode::COMMON)
        return;

    if (_selected != value)
    {
        _selected = value;
        setCurrentState();
        if (!_selectedTitle.empty() && _titleObject != nullptr)
            _titleObject->setText(_selected ? _selectedTitle : _title);
        if (!_selectedIcon.empty())
        {
            const AZStd::string& str = _selected ? _selectedIcon : _icon;
            if (_iconObject != nullptr)
                _iconObject->setIcon(str);
        }
        if (_relatedController != nullptr
            && getParent() != nullptr
            && !getParent()->_buildingDisplayList)
        {
            if (_selected)
            {
                _relatedController->setSelectedPageId(_relatedPageId);
                if (_relatedController->autoRadioGroupDepth)
                    getParent()->adjustRadioGroupDepth(this, _relatedController);
            }
            else if (_mode == ButtonMode::CHECK && _relatedController->getSelectedPageId().compare(_relatedPageId) == 0)
                _relatedController->setOppositePageId(_relatedPageId);
        }
    }
}

void GButton::setRelatedController(GController * c)
{
    _relatedController = c;
}

void GButton::setState(const AZStd::string& value)
{
    if (_buttonController != nullptr)
        _buttonController->setSelectedPage(value);

    if (_downEffect == 1)
    {
        if (value == DOWN)
        {
            if (!_downGray)
            {
                _downGray = true;
                for (auto element : this->getChildren())
                {
                    element->_color4b.r = element->_color4b.r * _downEffectValue;
                    element->_color4b.g = element->_color4b.g * _downEffectValue;
                    element->_color4b.b = element->_color4b.b * _downEffectValue;
                }
            }
        }
        else
        {
            if (_downGray)
            {
                _downGray = false;
                for (auto element : this->getChildren())
                {
                    element->_color4b.r = element->_color4b.r / _downEffectValue;
                    element->_color4b.g = element->_color4b.g / _downEffectValue;
                    element->_color4b.b = element->_color4b.b / _downEffectValue;
                }
            }
        }
    }
    else if (_downEffect == 2)
    {
        if (value == DOWN || value == SELECTED_OVER || value == SELECTED_DISABLED)
        {
            if (!_downScaled)
            {
                _downScaled = true;
                setScale(getScaleX() * _downEffectValue, getScaleY() * _downEffectValue);
            }
        }
        else
        {
            if (_downScaled)
            {
                _downScaled = false;
                setScale(getScaleX() / _downEffectValue, getScaleY() / _downEffectValue);
            }
        }
    }
}

void GButton::setCurrentState()
{
    if (isGrayed() && _buttonController != nullptr && _buttonController->hasPage(DISABLED))
    {
        if (_selected)
            setState(SELECTED_DISABLED);
        else
            setState(DISABLED);
    }
    else
    {
        if (_selected)
            setState(_over ? SELECTED_OVER : DOWN);
        else
            setState(_over ? OVER : UP);
    }
}

void GButton::constructFromXML(TXMLElement * xml)
{
    GComponent::constructFromXML(xml);

    xml = xml->FirstChildElement("Button");
    if (xml)
    {
        const char*p;
        p = xml->Attribute("mode");
        if (p)
            _mode = ToolSet::parseButtonMode(p);

        p = xml->Attribute("sound");
        if (p)
            _sound = p;

        p = xml->Attribute("volume");
        if (p)
            _soundVolumeScale = atof(p) / 100.0f;

        p = xml->Attribute("downEffect");
        if (p)
        {
            _downEffect = strcmp(p, "dark") == 0 ? 1 : (strcmp(p, "scale") == 0 ? 2 : 0);
            _downEffectValue = xml->FloatAttribute("downEffectValue");
            if (_downEffect == 2)
                setPivot(0.5f, 0.5f);
        }
    }

    _buttonController = getController("button");
    _titleObject = getChild("title");
    _iconObject = getChild("icon");
    if (_titleObject != nullptr)
        _title = _titleObject->getText();
    if (_iconObject != nullptr)
        _icon = _iconObject->getIcon();

    if (_mode == ButtonMode::COMMON)
        setState(UP);

    addEventListener(UIEventType::RollOver, FUI_CALLBACK_1(GButton::onRollOver, this));
    addEventListener(UIEventType::RollOut, FUI_CALLBACK_1(GButton::onRollOut, this));
    addEventListener(UIEventType::TouchBegin, FUI_CALLBACK_1(GButton::onTouchBegin, this));
    addEventListener(UIEventType::TouchEnd, FUI_CALLBACK_1(GButton::onTouchEnd, this));
    addEventListener(UIEventType::Click, FUI_CALLBACK_1(GButton::onClick, this));
    addEventListener(UIEventType::Exit, FUI_CALLBACK_1(GButton::onExit, this));
}

void GButton::setup_AfterAdd(TXMLElement * xml)
{
    GComponent::setup_AfterAdd(xml);

    xml = xml->FirstChildElement("Button");
    if (!xml)
        return;

    const char*p;

    p = xml->Attribute("title");
    if (p)
        setTitle(p);

    p = xml->Attribute("icon");
    if (p)
        setIcon(p);

    p = xml->Attribute("selectedTitle");
    if (p)
        setSelectedTitle(p);

    p = xml->Attribute("selectedIcon");
    if (p)
        setSelectedIcon(p);

    p = xml->Attribute("titleColor");
    if (p)
        setTitleColor(ToolSet::convertFromHtmlColor(p));

    p = xml->Attribute("titleFontSize");
    if (p)
        setTitleFontSize(atoi(p));

    p = xml->Attribute("controller");
    if (p)
        _relatedController = getParent()->getController(p);

    p = xml->Attribute("page");
    if (p)
        _relatedPageId = p;

    setSelected(xml->BoolAttribute("checked"));

    p = xml->Attribute("sound");
    if (p)
        _sound = p;

    p = xml->Attribute("volume");
    if (p)
        _soundVolumeScale = atof(p) / 100.0f;
}

void GButton::handleControllerChanged(GController* c)
{
    GObject::handleControllerChanged(c);

    if (_relatedController == c)
        setSelected(_relatedPageId.compare(c->getSelectedPageId()) == 0);
}

void GButton::onRollOver(EventContext* context)
{
    if (_buttonController == nullptr || !_buttonController->hasPage(OVER))
        return;

    _over = true;
    if (_down)
        return;

    if (isGrayed() && _buttonController->hasPage(DISABLED))
        return;

    setState(_selected ? SELECTED_OVER : OVER);
}

void GButton::onRollOut(EventContext* context)
{
    if (_buttonController == nullptr || !_buttonController->hasPage(OVER))
        return;

    _over = false;
    if (_down)
        return;

    if (isGrayed() && _buttonController->hasPage(DISABLED))
        return;

    setState(_selected ? DOWN : UP);
}

void GButton::onTouchBegin(EventContext* context)
{
    if (context->getInput()->getButton() != MouseButton::BUTTON_LEFT)
        return;

    _down = true;
    context->captureTouch();

    if (_mode == ButtonMode::COMMON)
    {
        if (isGrayed() && _buttonController != nullptr && _buttonController->hasPage(DISABLED))
            setState(SELECTED_DISABLED);
        else
            setState(DOWN);
    }
}

void GButton::onTouchEnd(EventContext* context)
{
    if (context->getInput()->getButton() != MouseButton::BUTTON_LEFT)
        return;

    if (_down)
    {
        _down = false;
        if (_mode == ButtonMode::COMMON)
        {
            if (isGrayed() && _buttonController != nullptr && _buttonController->hasPage(DISABLED))
                setState(DISABLED);
            else if (_over)
                setState(OVER);
            else
                setState(UP);
        }
        else
        {
            if (!_over
                && _buttonController != nullptr
                && (_buttonController->getSelectedPage() == OVER
                    || _buttonController->getSelectedPage() == SELECTED_OVER))
            {
                setCurrentState();
            }
        }
    }
}

void GButton::onClick(EventContext* context)
{
    if (!_sound.empty())
        UIRoot->playSound(_sound, _soundVolumeScale);

    if (_mode == ButtonMode::CHECK)
    {
        if (_changeStateOnClick)
        {
            setSelected(!_selected);
            dispatchEvent(UIEventType::Changed);
        }
    }
    else if (_mode == ButtonMode::RADIO)
    {
        if (_changeStateOnClick && !_selected)
        {
            setSelected(true);
            dispatchEvent(UIEventType::Changed);
        }
    }
    else
    {
        if (_relatedController != nullptr)
            _relatedController->setSelectedPageId(_relatedPageId);
    }
}

void GButton::onExit(EventContext * context)
{
    if (_over)
        onRollOut(context);
}

NS_FGUI_END
