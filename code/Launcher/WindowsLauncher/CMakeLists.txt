add_executable(WindowsLauncher WIN32 main.cpp)

target_link_libraries(WindowsLauncher LauncherBase
    AzCore
    Level Renderer Window
)

add_custom_command(TARGET WindowsLauncher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ENGINE_ASSETS_DIR} ${CMAKE_CURRENT_BINARY_DIR}/assets)