if(X_WINDOWS)
    add_executable(ObjectSample WIN32 ObjectSample.cc)
else()
    add_executable(ObjectSample ObjectSample.cc)
endif()

target_link_libraries(ObjectSample xEngine stb_image)

set_target_properties(ObjectSample PROPERTIES FOLDER "sample")