set(source_files
    lapi.c
    lauxlib.c
    lbaselib.c
    lbitlib.c
    lcode.c
    ldblib.c
    ldo.c
    ldump.c
    lfunc.c
    lgc.c
    linit.c
    liolib.c
    llex.c
    lmathlib.c
    lmem.c
    loadlib.c
    lobject.c
    lopcodes.c
    lparser.c
    lstate.c
    lstring.c
    lstrlib.c
    ltable.c
    ltablib.c
    ltm.c
    lundump.c
    lvm.c
    lzio.c
    print.c
    ldebug.c
    loslib.c
    compat.c
)

source_group(PREFIX "" FILES ${source_files} TREE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(lua ${source_files})

if (WIN32)
    target_compile_definitions(lua PRIVATE WIN32)
elseif (FIPS_ANDROID)
    add_definitions(-DANDROID)
elseif (FIPS_IOS)
    add_definitions(-DAPPLE -DAPPLETV)
elseif (FIPS_OSX)
    add_definitions(-DAPPLE)
else()
    add_definitions(-DLINUX)
endif()