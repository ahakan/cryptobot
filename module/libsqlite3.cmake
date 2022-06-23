if (UNIX)
    message(STATUS "Could not find SQLite3. This dependency will be downloaded.")

    set(SQLite3_PREFIX sqlite3)
    set(SQLite3_CONFIGURE ./configure)
    set(SQLite3_MAKE sudo make)
    set(SQLite3_INSTALL sudo make install)
    set(SQLite3_URL https://sqlite.org/2022/sqlite-autoconf-3380000.tar.gz)
    set(SQLite3_SHA3 7b1fb47d0a8137641b0a39e2345bb573b2b9e3e220d099854e1d8522fc449b9d)

    ExternalProject_Add(${SQLite3_PREFIX}
    PREFIX ${SQLite3_PREFIX}
    URL ${SQLite3_URL}
    URL_HASH SHA3_256=${SQLite3_SHA3}
    BUILD_IN_SOURCE true
    CONFIGURE_COMMAND ${SQLite3_CONFIGURE} 
    BUILD_COMMAND ${SQLite3_MAKE}
    INSTALL_COMMAND ${SQLite3_INSTALL})
else (UNIX)
    message(WARNING "Could not find SQLite3. You must install this dependency.")
endif (UNIX)

