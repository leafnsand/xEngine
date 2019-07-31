file(GLOB_RECURSE source_files src *.*)

add_library(bx ${source_files})

target_include_directories(bx PRIVATE 3rdparty)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(bx PRIVATE -DBX_CONFIG_DEBUG=1)
endif()