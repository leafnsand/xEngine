#pragma once
//------------------------------------------------------------------------------
/**
    @file spine/types.h
    @brief basic typedefs of the spine system
*/
#include "FairyGUIMacros.h"
#include <AzCore/std/functional.h>
#include <AzCore/std/string/string.h>

NS_FGUI_BEGIN

typedef AZStd::function<void()> AddPackageCallBack;
 
struct FairySystemDesc {
    AZStd::string AssetDirectory = "root:";
};

enum FairyRenderType
{
    Fairy,
    Spine,
    Particle2d,
    Shape,
    Mask,
    Font
};

enum FairyRenderMode
{
    ScreenSpaceOverlay,
    ScreenSpaceCamera,
    WorldSpace
};

enum class FairyFitScreenMode
{
    None,
    FitSize,
    FitWidthAndSetMiddle,
    FitHeightAndSetCenter
};

NS_FGUI_END
