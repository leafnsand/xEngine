cmake_minimum_required(VERSION 3.4)

project(xEngine)

set(CMAKE_CXX_STANDARD 14)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(X_ENGINE_OPTION_BUILD_PLUGIN "Build Plugin" ON)
option(X_ENGINE_OPTION_BUILD_SAMPLE "Build Sample" ON)
option(X_ENGINE_OPTION_BUILD_TOOL "Build Tool" ON)

option(X_ENGINE_OPTION_OPENGL "Use OpenGL" ON)
option(X_ENGINE_OPTION_D3D11 "Use DirectX3D 11" ON)

option(X_ENGINE_OPTION_GLAD_DEBUG "OpenGL Loader Debug Mode" ON)
option(X_ENGINE_OPTION_DEBUG "Debug Mode" ON)
option(X_ENGINE_OPTION_RTTI "Enable RTTI" OFF)

if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/platform/windows.cmake)
elseif(APPLE)
    include(${PROJECT_SOURCE_DIR}/cmake/platform/macOS.cmake)
else()
    include(${PROJECT_SOURCE_DIR}/cmake/platform/linux.cmake)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/build/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/build/bin)

add_subdirectory(runtime)
add_subdirectory(third_party)

if(X_ENGINE_OPTION_BUILD_PLUGIN)
    add_subdirectory(plugin)
endif()

if(X_ENGINE_OPTION_BUILD_SAMPLE)
    add_subdirectory(sample)
endif()

if(X_ENGINE_OPTION_BUILD_TOOL)
    add_subdirectory(tool)
endif()