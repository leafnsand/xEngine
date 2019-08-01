file(GLOB_RECURSE source_files ${CMAKE_CURRENT_SOURCE_DIR}/*.*)

source_group(PREFIX "" FILES ${source_files} TREE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(AzCore ${source_files})

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(AzCore PRIVATE /bigobj)
endif()

target_link_libraries(AzCore bx bgfx bimg lua mongoose)