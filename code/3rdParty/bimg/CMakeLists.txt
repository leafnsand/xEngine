set(source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/image.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/image_gnf.cpp)

add_library(bimg ${source_files})

target_include_directories(bimg PRIVATE 3rdparty)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_include_directories(bgfx PRIVATE ${ENGINE_SOURCE_3RDPARTY_DIR}/bx/include/compat/msvc)
endif()

if (APPLE)
    target_include_directories(bx PRIVATE ${ENGINE_SOURCE_3RDPARTY_DIR}/bx/include/compat/osx)
endif()

target_compile_definitions(bimg PRIVATE __STDINT_LIMITS)
target_compile_definitions(bimg PRIVATE __STDINT_MACROS)
target_compile_definitions(bimg PRIVATE __STDC_LIMIT_MACROS)
target_compile_definitions(bimg PRIVATE __STDC_FORMAT_MACROS)
target_compile_definitions(bimg PRIVATE __STDC_CONSTANT_MACROS)