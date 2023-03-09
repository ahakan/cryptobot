if (UNIX)
    message(STATUS "Could not find CURL. This dependency will be downloaded.")

    set(CURL_PREFIX curl)
    set(CURL_CONFIGURE ./configure --with-openssl)
    set(CURL_MAKE sudo make)
    set(CURL_INSTALL sudo make install)
    set(CURL_URL https://curl.se/download/curl-7.82.0.tar.gz)

    ExternalProject_Add(${CURL_PREFIX}
    PREFIX ${CURL_PREFIX}
    URL ${CURL_URL}
    BUILD_IN_SOURCE true
    CONFIGURE_COMMAND ${CURL_CONFIGURE} 
    BUILD_COMMAND ${CURL_MAKE}
    INSTALL_COMMAND ${CURL_INSTALL})
else (UNIX)
    message(WARNING "Could not find CURL. You must install this dependency.")
endif (UNIX)

