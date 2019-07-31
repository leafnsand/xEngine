set(source_files
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

add_library(pbc ${source_files})

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set_target_properties(pbc PROPERTIES COMPILE_FLAGS "-Wno-sign-compare")
endif()