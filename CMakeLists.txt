cmake_minimum_required(VERSION 3.0.0)

project(xEngine)

get_filename_component(ENGINE_ROOT_DIR "." ABSOLUTE)
get_filename_component(ENGINE_ASSETS_DIR "${ENGINE_ROOT_DIR}/assets" ABSOLUTE)
get_filename_component(ENGINE_SOURCE_DIR "${ENGINE_ROOT_DIR}/code" ABSOLUTE)
get_filename_component(ENGINE_SOURCE_3RDPARTY_DIR "${ENGINE_SOURCE_DIR}/3rdParty" ABSOLUTE)
get_filename_component(ENGINE_SOURCE_AZCORE_DIR "${ENGINE_SOURCE_DIR}/AzCore" ABSOLUTE)
get_filename_component(ENGINE_SOURCE_LAUNCHER_DIR "${ENGINE_SOURCE_DIR}/Launcher" ABSOLUTE)
get_filename_component(ENGINE_SOURCE_MODULE_DIR "${ENGINE_SOURCE_DIR}/Module" ABSOLUTE)

include_directories(
    ${ENGINE_SOURCE_DIR}
    ${ENGINE_SOURCE_MODULE_DIR}
    ${ENGINE_SOURCE_3RDPARTY_DIR}
    ${ENGINE_SOURCE_3RDPARTY_DIR}/bx/include
    ${ENGINE_SOURCE_3RDPARTY_DIR}/bimg/include
    ${ENGINE_SOURCE_3RDPARTY_DIR}/bgfx/include
)

add_subdirectory(${ENGINE_SOURCE_3RDPARTY_DIR})

add_subdirectory(${ENGINE_SOURCE_AZCORE_DIR})

add_subdirectory(${ENGINE_SOURCE_LAUNCHER_DIR})

add_subdirectory(${ENGINE_SOURCE_MODULE_DIR})