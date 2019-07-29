fips_begin_lib(pbc)
    fips_files(
        alloc.c
        alloc.h
        array.c
        array.h
        bootstrap.c
        bootstrap.h
        context.c
        context.h
        decode.c
        descriptor.pbc.h
        map.c
        map.h
        pattern.c
        pattern.h
        pbc.h
        pbc-lua.c
        proto.c
        proto.h
        register.c
        rmessage.c
        stringpool.c
        stringpool.h
        varint.c
        varint.h
        wmessage.c
    )
fips_end_lib()

if (FIPS_CLANG)
    set_target_properties(pbc PROPERTIES COMPILE_FLAGS "-Wno-sign-compare")
endif()