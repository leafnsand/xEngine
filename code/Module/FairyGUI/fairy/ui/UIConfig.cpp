#include "UIConfig.h"
#include "utils/FileUtils.h"

NS_FGUI_BEGIN

FUI__Implement_Singleton(UIConfig)

UIConfig::UIConfig()
{
    emptyString = "";
    defaultFont = "";
    buttonSound = "";
    buttonSoundVolumeScale = 1;
    defaultScrollStep = 25;
    defaultScrollDecelerationRate = 0.967f;
    defaultScrollTouchEffect = true;
    defaultScrollBounceEffect = true;
    defaultScrollBarDisplay = ScrollBarDisplayType::DEFAULT;
    verticalScrollBar = "";
    horizontalScrollBar = "";
    touchDragSensitivity = 10;
    clickDragSensitivity = 2;
    touchScrollSensitivity = 20;
    defaultComboBoxVisibleItemCount = 10;
    globalModalWaiting = "";
    tooltipsWin = "";
    modalLayerColor = Color4F(0, 0, 0, 0.4f);
    bringWindowToFrontOnClick = true;
    windowModalWaiting = "";
    popupMenu = "";
    popupMenu_seperator = "";
}
UIConfig::~UIConfig()
{
    
}

void UIConfig::registerFont(const AZStd::string& aliasName, const AZStd::string& realName)
{
    FontNameItem fi;
    fi.name = realName;
    //bool tmp = FileUtils::getInstance()->isPopupNotify();
    //FileUtils::getInstance()->setPopupNotify(false);
    fi.ttf = FileUtils::getInstance()->isFileExist(realName);
    //FileUtils::getInstance()->setPopupNotify(tmp);
    _fontNames[aliasName] = fi;
}

const AZStd::string& UIConfig::getRealFontName(const AZStd::string& aliasName, bool* isTTF)
{
    unordered_map<AZStd::string, UIConfig::FontNameItem>::const_iterator it;
    if (aliasName.empty())
        it = _fontNames.find(UIConfig::defaultFont);
    else
        it = _fontNames.find(aliasName);
    if (it != _fontNames.end())
    {
        if(isTTF)
            *isTTF = it->second.ttf;
        return it->second.name;
    }
    else
    {
        if (isTTF)
            *isTTF = false;
        return aliasName;
    }
}

NS_FGUI_END

