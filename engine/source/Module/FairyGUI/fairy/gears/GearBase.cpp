#include "GearBase.h"
#include "GearDisplay.h"
#include "ui/GComponent.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN


bool GearBase::disableAllTweenEffect = false;

GearBase::GearBase(GObject * owner) :
    tweenTime(0.3f),
    tween(false),
    delay(0),
    easeType(Quad_EaseOut)
{
    _owner = owner;
}

GearBase::~GearBase()
{
}

void GearBase::setController(GController * value)
{
    if (value != _controller)
    {
        _controller = value;
        if (_controller != nullptr)
            init();
    }
}

void GearBase::init()
{
}

void GearBase::addStatus(const AZStd::string&  pageId, const AZStd::string& value)
{
}

void GearBase::apply()
{
}

void GearBase::updateState()
{
}

void GearBase::updateFromRelations(float dx, float dy)
{
}

void GearBase::setup(TXMLElement * xml)
{
    const char* p;
    p = xml->Attribute("controller");
    if (p)
    {
        _controller = _owner->getParent()->getController(AZStd::string(p));
        if (_controller == nullptr)
            return;
    }

    init();

    tween = xml->BoolAttribute("tween");

    p = xml->Attribute("ease");
    if (p)
        easeType = ToolSet::parseEaseType(p);

    p = xml->Attribute("duration");
    if (p)
        tweenTime = atof(p);
    p = xml->Attribute("delay");
    if (p)
        delay = atof(p);

    vector<AZStd::string> pages;
    p = xml->Attribute("pages");
    if (p)
        ToolSet::splitString(p, ',', pages);

    if (this->SafeCast<GearDisplay>())
    {
        ((GearDisplay*)this)->pages = pages;
    }
    else
    {
        if (!pages.empty())
        {
            vector<AZStd::string> values;
            p = xml->Attribute("values");
            if (p)
                ToolSet::splitString(p, '|', values);

            int cnt1 = (int)pages.size();
            int cnt2 = (int)values.size();
            AZStd::string str;
            for (int i = 0; i < cnt1; i++)
            {
                if (i < cnt2)
                    str = values[i];
                else
                    str = UIConfig::getInstance()->emptyString;
                addStatus(pages[i], str);
            }
        }

        p = xml->Attribute("default");
        if (p)
            addStatus(UIConfig::getInstance()->emptyString, p);
    }
}

NS_FGUI_END