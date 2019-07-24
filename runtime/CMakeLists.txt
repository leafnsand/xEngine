if(X_ENGINE_OPTION_DEBUG)
    add_definitions("-DX_DEBUG=1")
endif()

if(X_ENGINE_OPTION_OPENGL)
    add_definitions("-DX_OPENGL=1")
endif()

if(X_ENGINE_OPTION_D3D11 AND X_WINDOWS)
    add_definitions("-DX_D3D11=1")
endif()

set(GLAD_DIR "glad")

if(X_ENGINE_OPTION_GLAD_DEBUG)
    set(GLAD_DIR "glad-debug")
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/runtime
    ${PROJECT_SOURCE_DIR}/third_party/EASTL/include
    ${PROJECT_SOURCE_DIR}/third_party/EASTL/test/packages/EABase/include/Common
    ${PROJECT_SOURCE_DIR}/third_party/mongoose
    ${PROJECT_SOURCE_DIR}/third_party/glm/glm
    ${PROJECT_SOURCE_DIR}/third_party/${GLAD_DIR}/include
)

file(GLOB_RECURSE runtime_source *.h *.cc *.mm)

add_library(xEngine ${runtime_source})

target_link_libraries(xEngine EASTL mongoose)

if(X_ENGINE_OPTION_OPENGL)
    target_link_libraries(xEngine glad)
endif()

if(X_MACOS)
    find_library(COCOA_FRAMEWORK Cocoa)
    target_link_libraries(xEngine ${COCOA_FRAMEWORK})
endif()

foreach(file ${runtime_source})
    get_filename_component(group ${file} PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" group ${group})
    string(REPLACE "/" "\\" group ${group})
    source_group(${group} FILES ${file})
endforeach()