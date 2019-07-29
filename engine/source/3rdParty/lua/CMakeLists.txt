fips_begin_lib(lua)
    if (FIPS_WINDOWS)
        add_definitions(-DWIN32)
    elseif (FIPS_ANDROID)
        add_definitions(-DANDROID)
    elseif (FIPS_IOS)
        add_definitions(-DAPPLE -DAPPLETV)
    elseif (FIPS_OSX)
        add_definitions(-DAPPLE)
    else()
        add_definitions(-DLINUX)
    endif()
    fips_files(
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
fips_end_lib()
