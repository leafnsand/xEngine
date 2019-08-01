file(GLOB_RECURSE source_files ${CMAKE_CURRENT_SOURCE_DIR}/*.*)

add_library(Level ${source_files})
