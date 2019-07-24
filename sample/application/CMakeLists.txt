add_executable(ApplicationSample ApplicationSample.cc)

target_link_libraries(ApplicationSample xEngine)

set_target_properties(ApplicationSample PROPERTIES FOLDER "sample")