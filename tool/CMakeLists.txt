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
    ${PROJECT_SOURCE_DIR}/third_party/stb_image
    ${PROJECT_SOURCE_DIR}/third_party/nuklear
    ${PROJECT_SOURCE_DIR}/plugin
)

add_subdirectory(mesh_convert_tool)
add_subdirectory(shader_compile_tool)