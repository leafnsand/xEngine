#ifndef __TOOLSET_H__
#define __TOOLSET_H__

#include "base/Base.h"
#include "ui/Transition.h"
#include "ui/PackageItem.h"

NS_FGUI_BEGIN
using namespace tweenfunc;

class ToolSet
{
public:
    static void splitString(const AZStd::string &s, char delim, vector<AZStd::string> &elems);
    static void splitString(const AZStd::string &s, char delim, Vec2& value, bool intType = false);
    static void splitString(const AZStd::string &s, char delim, Vec4& value, bool intType = false);
    static void splitString(const AZStd::string &s, char delim, AZStd::string& str1, AZStd::string& str2);
    static int findInStringArray(const vector<AZStd::string>& arr, const AZStd::string& str);

    static Color4B convertFromHtmlColor(const char* str);

    static PackageItemType parsePackageItemType(const char * p);
    static TextHAlignment parseAlign(const char *p);
    static TextVAlignment parseVerticalAlign(const char *p);
    static int parseGearIndex(const char* p);
    static LoaderFillType parseFillType(const char *p);
    static ButtonMode parseButtonMode(const char*p);
    static OverflowType parseOverflowType(const char*p);
    static ScrollType parseScrollType(const char*p);
    static ScrollBarDisplayType parseScrollBarDisplayType(const char*p);
    static ProgressTitleType parseProgressTitleType(const char*p);
    static ListLayoutType parseListLayoutType(const char*p);
    static ListSelectionMode parseListSelectionMode(const char*p);
    static ChildrenRenderOrder parseChildrenRenderOrder(const char*p);
    static GroupLayoutType parseGroupLayoutType(const char*p);
    static PopupDirection parsePopupDirection(const char*p);
    static TextAutoSize parseTextAutoSize(const char*p);
    static FlipType parseFlipType(const char*p);
    static TransitionActionType parseTransitionActionType(const char*p);
    static TweenType parseEaseType(const char*p);
    static FillMethod parseFillMethod(const char * p);
};

class FastSplitter
{
public:
    FastSplitter();
    void start(const char* data, ssize_t dataLength, char delimiter);
    bool next();
    const char* getText();
    ssize_t getTextLength();
    void getKeyValuePair(char* keyBuf, ssize_t keyBufSize, char* valueBuf, ssize_t valueBufSize);

private:
    const char* data;
    ssize_t dataLength;
    ssize_t textLength;
    char delimiter;
};

NS_FGUI_END

#endif
