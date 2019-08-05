add_executable(MacLauncher MACOSX_BUNDLE MacApplication.h MacApplication.mm MacApplicationDelegate.mm main.mm)

find_library(CoreFoundationLib CoreFoundation)
find_library(CoreGraphicsLib CoreGraphics)
find_library(CarbonLib Carbon)
find_library(MetalLib Metal)
find_library(QuartzCoreLib QuartzCore)
find_library(AppKitLib AppKit)
find_library(GameControllerLib GameController)

target_link_libraries(MacLauncher LauncherBase
    AzCore ${CoreFoundationLib} ${CoreGraphicsLib} ${CarbonLib} ${MetalLib} ${QuartzCoreLib} ${AppKitLib} ${GameControllerLib}
    Level Renderer Window
)

add_custom_command(TARGET MacLauncher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ENGINE_ASSETS_DIR} ${CMAKE_CURRENT_BINARY_DIR}/MacLauncher.app/Contents/Resources/assets)