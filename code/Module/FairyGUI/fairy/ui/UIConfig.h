#ifndef __UICONFIG_H__
#define __UICONFIG_H__

#include "FairyGUIMacros.h"

#include "base/Base.h"
#include "FieldTypes.h"

NS_FGUI_BEGIN

class UIConfig
{
    FUI_Declare_Singleton(UIConfig)
public:
    UIConfig();
    virtual ~UIConfig();

    AZStd::string emptyString;
    AZStd::string defaultFont;
    AZStd::string buttonSound;
    float buttonSoundVolumeScale;
    int defaultScrollStep;
    float defaultScrollDecelerationRate;
    bool defaultScrollTouchEffect;
    bool defaultScrollBounceEffect;
    ScrollBarDisplayType defaultScrollBarDisplay;
    AZStd::string verticalScrollBar;
    AZStd::string horizontalScrollBar;
    int touchDragSensitivity;
    int clickDragSensitivity;
    int touchScrollSensitivity;
    int defaultComboBoxVisibleItemCount;
    AZStd::string globalModalWaiting;
    Color4F modalLayerColor;
    AZStd::string tooltipsWin;
    bool bringWindowToFrontOnClick;
    AZStd::string windowModalWaiting;
    AZStd::string popupMenu;
    AZStd::string popupMenu_seperator;

    void registerFont(const AZStd::string& aliasName, const AZStd::string& realName);
    const AZStd::string& getRealFontName(const AZStd::string& aliasName, bool* isTTF = nullptr);

private:
    struct FontNameItem
    {
        AZStd::string name;
        bool ttf;
    };
    unordered_map<AZStd::string, FontNameItem> _fontNames;
};

NS_FGUI_END

#endif
