if(X_WINDOWS)
  add_executable(WindowSample WIN32 WindowSample.cc)
else()
  add_executable(WindowSample WindowSample.cc)
endif()

target_link_libraries(WindowSample xEngine)

set_target_properties(WindowSample PROPERTIES FOLDER "sample")