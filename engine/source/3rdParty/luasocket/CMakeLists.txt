if (FIPS_WINDOWS)
    fips_begin_lib(luasocket)
        fips_files(
            auxiliar.c auxiliar.h
            buffer.c buffer.h
            except.c except.h
            inet.c inet.h
            io.c io.h
            luasocket.c luasocket.h
            options.c options.h
            pierror.h
            select.c select.h
            socket.h
            tcp.c tcp.h
            timeout.c timeout.h
            udp.c udp.h
            wsocket.c wsocket.h
        )
        fips_deps(lua)
    fips_end_lib()
    target_include_directories(luasocket PRIVATE ${FIPS_PROJECT_DIR}/code/3rdparty/lua)
    target_compile_definitions(luasocket PRIVATE LUASOCKET_API=)
endif()