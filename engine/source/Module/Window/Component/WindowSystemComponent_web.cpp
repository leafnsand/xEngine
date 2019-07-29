#include <AzCore/PlatformDef.h>

#if defined(AZ_PLATFORM_EMSCRIPTEN)

#include <Window/EBus/WindowSystemComponentBus.h>

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

EM_BOOL emscWindowSizeChanged(int eventType, const EmscriptenUiEvent* uiEvent, void* userData) {
    double width, height;
    emscripten_get_element_css_size("canvas", &width, &height);
    EBUS_EVENT(Module::WindowsSystemNotificationBus, OnWindowSizeChanged, int(width), int(height));
    return true;
}

void* CreateNativeWindow(int& width, int& height)
{
    double w, h;
    emscripten_get_element_css_size("canvas", &w, &h);
    width = int(w);
    height = int(h);

    emscripten_set_resize_callback(nullptr, nullptr, false, emscWindowSizeChanged);

    return nullptr;
}

#endif