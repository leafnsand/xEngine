file(GLOB_RECURSE source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.*)

add_library(bx ${source_files})

target_include_directories(bx PRIVATE 3rdparty)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_include_directories(bx PRIVATE include/compat/msvc)
endif()

if (APPLE)
    target_include_directories(bx PRIVATE include/compat/osx)
endif()

target_compile_definitions(bx PRIVATE __STDINT_LIMITS)
target_compile_definitions(bx PRIVATE __STDINT_MACROS)
target_compile_definitions(bx PRIVATE __STDC_LIMIT_MACROS)
target_compile_definitions(bx PRIVATE __STDC_FORMAT_MACROS)
target_compile_definitions(bx PRIVATE __STDC_CONSTANT_MACROS)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(bx PRIVATE BX_CONFIG_DEBUG=1)
endif()