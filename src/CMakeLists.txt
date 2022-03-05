include_directories(${CMAKE_SOURCE_DIR}/inc)

find_package(Threads REQUIRED)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS) # header only libraries must not be added here
include_directories(${Boost_INCLUDE_DIRS}) 


find_package(CURL REQUIRED) 
if(CURL_FOUND)
    message(STATUS "Found CURL version: ${CURL_VERSION_STRING}")
    message(STATUS "Using CURL include dir(s): ${CURL_INCLUDE_DIRS}")
    message(STATUS "Using CURL lib(s): ${CURL_LIBRARIES}")
    include_directories(${CURL_INCLUDE_DIRS})
else()
    message(WARNING "Could not find libcurl and libcurl-openssl. This dependency will be downloaded. To avoid this you can install curl yourself using the standard methods for your platform.")
endif()

find_package(OpenSSL REQUIRED) 
if(OPENSSL_FOUND)
    message(STATUS "Found SSL version: ${OPENSSL_VERSION}")
    message(STATUS "Using SSL include dir(s): ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "Using SSL lib(s): ${OPENSSL_LIBRARIES}")
    include_directories(${OPENSSL_INCLUDE_DIR})
else()
    message(WARNING "Could not find libssl and libcrypto. This dependency will be downloaded. To avoid this you can install curl yourself using the standard methods for your platform.")
endif()


set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

set(SOURCES
  ${SOURCE_DIR}/main.cpp
  ${SOURCE_DIR}/opel.cpp
  ${SOURCE_DIR}/sql.cpp
  ${SOURCE_DIR}/utilities.cpp
  ${SOURCE_DIR}/websocket.cpp
  ${SOURCE_DIR}/requests.cpp
)

add_executable(cryptobot ${SOURCES}) 
target_link_libraries(cryptobot sub::libelog sub::libjson sub::libsqlite -lcap ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} ${CURL_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
