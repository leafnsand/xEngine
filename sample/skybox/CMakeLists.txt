if(X_WINDOWS)
    add_executable(SkyBox WIN32 SkyBoxSample.cc)
else()
    add_executable(SkyBox SkyBoxSample.cc)
endif()

target_link_libraries(SkyBox xEngine stb_image)

set_target_properties(SkyBox PROPERTIES FOLDER "sample")