file(GLOB_RECURSE source_files ${CMAKE_CURRENT_SOURCE_DIR}/*.*)

source_group(PREFIX "" FILES ${source_files} TREE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(WindowsLauncher WIN32 ${source_files})

target_link_libraries(WindowsLauncher LauncherBase
    AzCore
    Level Renderer Window
)

add_custom_command(TARGET WindowsLauncher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ENGINE_ASSETS_DIR} ${CMAKE_CURRENT_BINARY_DIR}/assets)