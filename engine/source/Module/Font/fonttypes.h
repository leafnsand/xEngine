#pragma once
//------------------------------------------------------------------------------
/**
    @file n3font/fonttypes.h
    @brief public types of the n3font system
*/
#include <AzCore/std/string/string.h>
#include <AzCore/std/containers/vector.h>

namespace N3Font {

struct OriginType {
    enum Code {
        OriginTopLeft,
        OriginBottomLeft,
    };
};

struct FontSize {
    enum {
        Small = 0,
        Medium,
        Large,
        Num
    };
};

struct FontNamePath
{
    AZStd::string name = "";
    AZStd::string path = "";
};

struct FontSetup {
    int TextureWidth = 512;
    int TextureHeight = 512;
    OriginType::Code Origin = OriginType::OriginBottomLeft;
    AZStd::vector<FontNamePath> TTFFontPath;
};

} // namespace N3Font
