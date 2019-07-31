get_filename_component(ENGINE_ROOT_DIR "." ABSOLUTE)

include_directories(
    source
    source/Module
    source/3rdParty
    source/3rdParty/bx/include
    source/3rdParty/bimg/include
    source/3rdParty/bgfx/include
)

add_subdirectory(source/AzCore)

add_subdirectory(source/Launcher)

#add_subdirectory(source/Module)

add_subdirectory(source/3rdParty)