if(X_WINDOWS)
    add_executable(MeshSample WIN32 MeshSample.cc)
else()
    add_executable(MeshSample MeshSample.cc)
endif()

target_link_libraries(MeshSample xEngine)

set_target_properties(MeshSample PROPERTIES FOLDER "sample")