file(GLOB_RECURSE source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.*)

add_library(bgfx ${source_files})

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_DEBUG=1)
endif()

target_include_directories(bgfx PRIVATE
    3rdparty
    3rdparty/dxsdk/include
    3rdparty/khronos
)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_include_directories(bgfx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../bx/include/compat/msvc)
endif()

target_compile_definitions(bgfx PRIVATE __STDINT_LIMITS)
target_compile_definitions(bgfx PRIVATE __STDINT_MACROS)
target_compile_definitions(bgfx PRIVATE __STDC_LIMIT_MACROS)
target_compile_definitions(bgfx PRIVATE __STDC_FORMAT_MACROS)
target_compile_definitions(bgfx PRIVATE __STDC_CONSTANT_MACROS)

if (FIPS_OSX)
    fips_frameworks_osx(Cocoa IOKit CoreFoundation CoreVideo Carbon OpenGL Metal QuartzCore)
elseif (WIN32)
    target_link_libraries(bgfx opengl32 psapi)
elseif (FIPS_LINUX)
    fips_libs(X11 Xrandr Xi Xinerama Xxf86vm Xcursor GL dl)
elseif (FIPS_ANDROID)
    fips_libs(GLESv2 EGL)
elseif (FIPS_IOS)
    fips_frameworks_osx(OpenGLES GLKit Metal CoreGraphics QuartzCore)
endif()