find_package(SQLite3 REQUIRED) 

if(SQLite3_FOUND)
    message(STATUS "Found SQLite3 version: ${SQLite3_VERSION}")
    message(STATUS "Using SQLite3 include dir(s): ${SQLite3_INCLUDE_DIR}")
    message(STATUS "Using SQLite3 lib(s): ${SQLite3_LIBRARIES}")
    include_directories(${SQLite3_INCLUDE_DIR})
else()
    if (UNIX)
        message(STATUS "Could not find SQLite3. This dependency will be downloaded.")

        set(SQLite3_PREFIX sqlite3)
        set(SQLite3_CONFIGURE ./configure)
        set(SQLite3_MAKE make)
        set(SQLite3_INSTALL make install)
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
endif()

