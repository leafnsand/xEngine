#include <AzCore/PlatformDef.h>

#if defined(AZ_PLATFORM_ANDROID)

#include <AzCore/Android/Utils.h>

void* CreateNativeWindow(int& width, int& height)
{
    AZ::Android::Utils::GetWindowSize(width, height);
    return AZ::Android::Utils::GetWindow();
}

#endif