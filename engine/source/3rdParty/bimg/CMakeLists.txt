fips_begin_lib(bimg)
    fips_include_directories(3rdparty)
    fips_dir(include/bimg)
    fips_files(bimg.h)
    fips_dir(src)
    fips_files(bimg_p.h image.cpp image_gnf.cpp)
fips_end_lib()

if (N3H5_BUILD_TOOLS)
    if (FIPS_WINDOWS OR FIPS_OSX OR FIPS_LINUX)
        fips_begin_lib(bimg_encode)
            fips_include_directories(
                3rdparty
                3rdparty/nvtt
                3rdparty/iqa/include
            )
            fips_dir(include/bimg)
            fips_files(bimg.h encode.h)
            fips_dir(src)
            fips_files(image_encode.cpp)
            fips_src(3rdparty/libsquish GROUP_FOLDERS)
            fips_src(3rdparty/edtaa3 GROUP_FOLDERS)
            fips_src(3rdparty/etc1 GROUP_FOLDERS)
            fips_src(3rdparty/etc2 GROUP_FOLDERS)
            fips_src(3rdparty/nvtt GROUP_FOLDERS)
            fips_src(3rdparty/pvrtc GROUP_FOLDERS)
            fips_src(3rdparty/tinyexr GROUP_FOLDERS)
            fips_src(3rdparty/iqa GROUP_FOLDERS)
        fips_end_lib()

        if (FIPS_MSVC)
            set_target_properties(bimg_encode PROPERTIES COMPILE_FLAGS "/wd4819")
        endif()

        fips_begin_lib(bimg_decode)
            fips_include_directories(
                3rdparty
                3rdparty/nvtt
                3rdparty/iqa/include
            )
            fips_dir(include/bimg)
            fips_files(bimg.h decode.h)
            fips_dir(src)
            fips_files(image_decode.cpp)
        fips_end_lib()

        fips_begin_app(texturec cmdline)
            fips_src(tools/texturec GROUP_FOLDERS)
            fips_deps(bx bimg bimg_encode bimg_decode)
        fips_end_app()
    endif()
endif()