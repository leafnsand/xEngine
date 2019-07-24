add_executable(StorageSample StorageSample.cc)

target_link_libraries(StorageSample xEngine)

set_target_properties(StorageSample PROPERTIES FOLDER "sample")