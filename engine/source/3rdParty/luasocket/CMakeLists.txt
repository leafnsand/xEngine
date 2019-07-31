if (WIN32)
    set(source_files
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

    add_library(luasocket ${source_files})

    target_include_directories(luasocket PRIVATE ../lua)

    target_compile_definitions(luasocket PRIVATE LUASOCKET_API=)
endif()