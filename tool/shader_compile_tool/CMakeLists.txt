file(GLOB_RECURSE shader_compile_tool_source *.h *.cc)

if(X_WINDOWS)
    add_executable(ShaderCompileTool WIN32 ${shader_compile_tool_source})
else()
    add_executable(ShaderCompileTool ${shader_compile_tool_source})
endif()

target_link_libraries(ShaderCompileTool xEngine xEngineNuklear)

set_target_properties(ShaderCompileTool PROPERTIES FOLDER "tool")