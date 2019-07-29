fips_begin_lib(bgfx)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        add_definitions("-DBGFX_CONFIG_DEBUG=1")
    endif()
    fips_include_directories(
        3rdparty
        3rdparty/dxsdk/include
        3rdparty/khronos
    )
    fips_src(include GROUP_FOLDERS)
    fips_src(src EXCEPT amalgamated.cpp amalgamated.mm GROUP_FOLDERS)
    if (FIPS_OSX)
        fips_frameworks_osx(Cocoa IOKit CoreFoundation CoreVideo Carbon OpenGL Metal QuartzCore)
    elseif (FIPS_WINDOWS)
        fips_libs(opengl32 psapi)
    elseif (FIPS_LINUX)
        fips_libs(X11 Xrandr Xi Xinerama Xxf86vm Xcursor GL dl)
    elseif (FIPS_ANDROID)
        fips_libs(GLESv2 EGL)
    elseif (FIPS_IOS)
        fips_frameworks_osx(OpenGLES GLKit Metal CoreGraphics QuartzCore)
    endif()
fips_end_lib()

if (N3H5_BUILD_TOOLS)
    if (FIPS_WINDOWS OR FIPS_OSX OR FIPS_LINUX)
        fips_begin_lib(bgfx-glslang)
            add_definitions(-DENABLE_HLSL=1)
            fips_include_directories(3rdparty/glslang)
            file(GLOB_RECURSE glslang_sources
                3rdparty/glslang/glslang/*.cpp
                3rdparty/glslang/glslang/*.h
                3rdparty/glslang/hlsl/*.cpp
                3rdparty/glslang/hlsl/*.h
                3rdparty/glslang/SPIRV/*.cpp
                3rdparty/glslang/SPIRV/*.h
                3rdparty/glslang/OGLCompilersDLL/*.cpp
                3rdparty/glslang/OGLCompilersDLL/*.h
            )
            file(GLOB_RECURSE glslang_sources_remove
                3rdparty/glslang/glslang/OSDependent/Unix/main.cpp
                3rdparty/glslang/glslang/OSDependent/Windows/main.cpp
            )
            list(REMOVE_ITEM glslang_sources ${glslang_sources_remove})
            if (FIPS_MSVC)
                file(GLOB_RECURSE glslang_sources_remove_msvc
                    3rdparty/glslang/glslang/OSDependent/Unix/*.cpp
                    3rdparty/glslang/glslang/OSDependent/Unix/*.h
                )
                list(REMOVE_ITEM glslang_sources ${glslang_sources_remove_msvc})
            else()
                file(GLOB_RECURSE glslang_sources_remove_not_msvc
                    3rdparty/glslang/glslang/OSDependent/Windows/*.cpp
                    3rdparty/glslang/glslang/OSDependent/Windows/*.h
                )
                list(REMOVE_ITEM glslang_sources ${glslang_sources_remove_not_msvc})
            endif()
            fips_files(${glslang_sources})
        fips_end_lib()

        if (FIPS_MSVC)
            set_target_properties(bgfx-glslang PROPERTIES COMPILE_FLAGS "/wd4005 /wd4100 /wd4127 /wd4244 /wd4456 /wd4457 /wd4458 /wd4702 /wd4819")
        else()
            set_target_properties(bgfx-glslang PROPERTIES COMPILE_FLAGS "-Wno-ignored-qualifiers -Wno-missing-field-initializers -Wno-reorder -Wno-return-type -Wno-shadow -Wno-sign-compare -Wno-undef -Wno-unknown-pragmas -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable")
        endif()

        fips_begin_lib(bgfx-fcpp)
            add_definitions(
                -DNINCLUDE=64
                -DNWORK=65536
                -DNBUFF=65536
                -DOLD_PREPROCESSOR=0
            )
            fips_include_directories(3rdparty/fcpp)
            fips_src(3rdparty/fcpp EXCEPT usecpp.c)
        fips_end_lib()

        if (FIPS_MSVC)
            set_target_properties(bgfx-fcpp PROPERTIES COMPILE_FLAGS "/wd4055 /wd4244 /wd4701 /wd4706")
        else()
            set_target_properties(bgfx-fcpp PROPERTIES COMPILE_FLAGS "-Wno-implicit-fallthrough")
        endif()

        fips_begin_lib(bgfx-glsl-optimizer)
            fips_include_directories(
                3rdparty/glsl-optimizer/include
                3rdparty/glsl-optimizer/src/mesa
                3rdparty/glsl-optimizer/src/mapi
                3rdparty/glsl-optimizer/src/glsl
                3rdparty/glsl-optimizer/src
            )
            file(GLOB_RECURSE glsl_sources
                3rdparty/glsl-optimizer/src/glsl/glcpp/*.c
                3rdparty/glsl-optimizer/src/glsl/*.cpp
                3rdparty/glsl-optimizer/src/glsl/*.c
                3rdparty/glsl-optimizer/src/mesa/*.c
                3rdparty/glsl-optimizer/src/mesa/main/*.c
                3rdparty/glsl-optimizer/src/mesa/program/*.c
                3rdparty/glsl-optimizer/src/util/*.c
            )
            file(GLOB_RECURSE glsl_sources_remove
                3rdparty/glsl-optimizer/src/glsl/glcpp/glcpp.cpp
                3rdparty/glsl-optimizer/src/glsl/ir_set_program_inouts.cpp
                3rdparty/glsl-optimizer/src/glsl/main.cpp
                3rdparty/glsl-optimizer/src/glsl/builtin_stubs.cpp
            )
            list(REMOVE_ITEM glsl_sources ${glsl_sources_remove})
            fips_files(${glsl_sources})
        fips_end_lib()

        if (FIPS_MSVC)
            set_target_properties(bgfx-glsl-optimizer PROPERTIES COMPILE_FLAGS "/wd4291 /WX-")
        endif()

        fips_begin_app(shaderc cmdline)
            fips_include_directories(
                3rdparty/dxsdk/include
                3rdparty/fcpp
                3rdparty/glslang/glslang/Public
                3rdparty/glslang/glslang/Include
                3rdparty/glslang
                3rdparty/glsl-optimizer/include
                3rdparty/glsl-optimizer/src/glsl
            )
            fips_src(tools/shaderc GROUP_FOLDERS)
            fips_files_ex(src vertexdecl.* GROUP_FOLDERS)
            fips_files_ex(src shader_spirv.* GROUP_FOLDERS)
            fips_deps(bx bgfx-fcpp bgfx-glslang bgfx-glsl-optimizer)
            if (FIPS_WINDOWS)
                add_definitions(
                    -D__STDC__
                    -D__STDC_CONSTANT_MACROS
                    -D__STDC_LIMIT_MACROS
                    -D__STDC_VERSION__=199901L
                    -Dstrdup=_strdup
                    -Dalloca=_alloca
                    -Disascii=__isascii
                )
                fips_include_directories(
                    3rdparty/glsl-optimizer/include/c99
                    3rdparty/dxsdk/include
                )
                set(DXSDK_LIB "$ENV{DXSDK_DIR}Lib\\${FIPS_WINDOWS_PLATFORM_NAME}\\")
                fips_libs(psapi)
            endif()
        fips_end_app()

        if (FIPS_CLANG)
            set_target_properties(shaderc PROPERTIES COMPILE_FLAGS "-Wno-strict-aliasing")
        elseif(FIPS_GCC)
            set_target_properties(shaderc PROPERTIES COMPILE_FLAGS "-Wno-strict-aliasing")
        elseif(FIPS_MSVC)
            set_target_properties(shaderc PROPERTIES LINK_FLAGS "/LIBPATH:\"${DXSDK_LIB}\\\"")
            set_target_properties(shaderc PROPERTIES COMPILE_FLAGS "/wd4152 /wd4204 /wd4242 /wd4244 /wd4668 /wd4996 /wd4291 /WX-")
        endif()
    endif()
endif()