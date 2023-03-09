if (UNIX)
    message(STATUS "Libcap2 will be downloaded.")

    set(libcap2_PREFIX libcap)
    set(libcap2_INSTALL sudo make install)
    set(libcap2_URL https://mirrors.edge.kernel.org/pub/linux/libs/security/linux-privs/libcap2/libcap-2.63.tar.gz)
    set(libcap2_SHA256 f968e9b8328a0ce391ea7d51ce720346cd20c6891a6fd81edf0e2660224c51c7)

    ExternalProject_Add(${libcap2_PREFIX}
    PREFIX ${libcap2_PREFIX}
    URL ${libcap2_URL}
    URL_HASH SHA256=${libcap2_SHA256}
    BUILD_IN_SOURCE true
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${libcap2_INSTALL}
    INSTALL_COMMAND "")
else (UNIX)
    message(WARNING "Could not install libcap2. You must install this dependency.")
endif (UNIX)


