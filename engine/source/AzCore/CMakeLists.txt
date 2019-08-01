file(GLOB_RECURSE source_files ${CMAKE_CURRENT_SOURCE_DIR}/*.*)

source_group(PREFIX "" FILES ${source_files} TREE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(AzCore ${source_files})

target_link_libraries(AzCore bx bgfx bimg lua mongoose)