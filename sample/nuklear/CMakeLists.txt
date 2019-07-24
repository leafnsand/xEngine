if(X_WINDOWS)
    add_executable(NuklearSample WIN32 NuklearSample.cc)
else()
    add_executable(NuklearSample NuklearSample.cc)
endif()

target_link_libraries(NuklearSample xEngine xEngineNuklear)

set_target_properties(NuklearSample PROPERTIES FOLDER "sample")