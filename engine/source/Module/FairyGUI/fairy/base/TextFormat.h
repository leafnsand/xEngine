#ifndef __TEXTFORMAT_H__
#define __TEXTFORMAT_H__

#include "FairyGUIMacros.h"

#include "Types.h"
#include "Geometry.h"


#include "Ref.h"

NS_FGUI_BEGIN

class TextFormat : public Ref
{
public:
    TextFormat();
    void setFormat(const TextFormat& format);

    void enableEffect(int effectFlag) { effect |= effectFlag; }
    void disableEffect(int effectFlag) { effect &= ~effectFlag; }
    bool hasEffect(int effectFlag) const { return (effect & effectFlag) != 0; }

    static const int OUTLINE = 1;
    static const int SHADOW = 2;
    static const int GLOW = 4;

    AZStd::string face;
    float fontSize;
    Color4B color;
    bool bold;
    bool italics;
    bool underline;
    int lineSpacing;
    int letterSpacing;
    TextHAlignment align;
    TextVAlignment verticalAlign;

    int effect;
    Color3B outlineColor;
    int outlineSize;
    Color3B shadowColor;
    Size shadowOffset;
    int shadowBlurRadius;
    Color3B glowColor;

    //internal use
    bool _hasColor;
};

NS_FGUI_END

#endif
