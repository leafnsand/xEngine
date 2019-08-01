file(GLOB_RECURSE source_files ${CMAKE_CURRENT_SOURCE_DIR}/*.*)

add_library(Renderer ${source_files})

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_include_directories(Renderer PRIVATE ${ENGINE_SOURCE_3RDPARTY_DIR}/bx/include/compat/msvc)
endif()