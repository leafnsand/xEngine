if(X_WINDOWS)
  add_executable(DepthStencilSample WIN32 DepthStencilSample.cc)
else()
  add_executable(DepthStencilSample DepthStencilSample.cc)
endif()

target_link_libraries(DepthStencilSample xEngine stb_image)

set_target_properties(DepthStencilSample PROPERTIES FOLDER "sample")