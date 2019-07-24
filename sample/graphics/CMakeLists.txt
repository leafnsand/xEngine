if(X_WINDOWS)
  add_executable(GraphicsSample WIN32 GraphicsSample.cc)
else()
  add_executable(GraphicsSample GraphicsSample.cc)
endif()

target_link_libraries(GraphicsSample xEngine stb_image)

set_target_properties(GraphicsSample PROPERTIES FOLDER "sample")