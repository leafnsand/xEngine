include_directories(${PROJECT_SOURCE_DIR}/third_party/assimp/include)
link_directories(${PROJECT_SOURCE_DIR}/third_party/assimp/lib/${CMAKE_BUILD_TYPE}/${X_PLATFORM_NAME}/)

file(GLOB_RECURSE mesh_convert_tool_source *.h *.cc)

if(X_WINDOWS)
    add_executable(MeshConvertTool WIN32 ${mesh_convert_tool_source})
else()
    add_executable(MeshConvertTool ${mesh_convert_tool_source})
endif()

target_link_libraries(MeshConvertTool xEngine xEngineNuklear assimp stb_image)

add_custom_command(TARGET MeshConvertTool POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/third_party/assimp/bin/${CMAKE_BUILD_TYPE}/${X_PLATFORM_NAME}/libassimp${CMAKE_SHARED_LIBRARY_SUFFIX}"
    $<TARGET_FILE_DIR:MeshConvertTool>)

set_target_properties(MeshConvertTool PROPERTIES FOLDER "tool")