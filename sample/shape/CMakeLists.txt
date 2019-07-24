if(X_WINDOWS)
    add_executable(ShapeSample WIN32 ShapeSample.cc)
else()
    add_executable(ShapeSample ShapeSample.cc)
endif()

target_link_libraries(ShapeSample xEngine stb_image)

set_target_properties(ShapeSample PROPERTIES FOLDER "sample")