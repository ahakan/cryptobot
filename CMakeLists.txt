cmake_minimum_required(VERSION 3.6.0)
project(cryptobot VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-format-security")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)
include(ExternalProject)

SET(BUILD_LIBCAP TRUE CACHE BOOL FALSE)
SET(BUILD_SQLITE3 TRUE CACHE BOOL FALSE)
SET(BUILD_CURL TRUE CACHE BOOL FALSE)

if(BUILD_LIBCAP STREQUAL TRUE)
    include(module/libcap2.cmake)
endif()

find_package(SQLite3) 
if(SQLite3_FOUND)
    message(STATUS "Found SQLite3 version: ${SQLite3_VERSION}")
    message(STATUS "Using SQLite3 include dir(s): ${SQLite3_INCLUDE_DIR}")
    message(STATUS "Using SQLite3 lib(s): ${SQLite3_LIBRARIES}")
    include_directories(${SQLite3_INCLUDE_DIR})
else()
    if(BUILD_SQLITE3 STREQUAL TRUE)
        include(module/libsqlite3.cmake)
    endif()
endif()

find_package(CURL) 
if(CURL_FOUND)
    message(STATUS "Found CURL version: ${CURL_VERSION_STRING}")
    message(STATUS "Using CURL include dir(s): ${CURL_INCLUDE_DIRS}")
    message(STATUS "Using CURL lib(s): ${CURL_LIBRARIES}")
    include_directories(${CURL_INCLUDE_DIRS})
else()
    if(BUILD_CURL STREQUAL TRUE)
        include(module/libcurl.cmake)
    endif()
endif()

find_package(OpenSSL) 
if(OPENSSL_FOUND)
    message(STATUS "Found OpenSSL version: ${OPENSSL_VERSION}")
    message(STATUS "Using OpenSSL include dir(s): ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "Using OpenSSL lib(s): ${OPENSSL_LIBRARIES}")
    include_directories(${OPENSSL_INCLUDE_DIR})
else()
    message(WARNING "Could not find libssl and libcrypto. This dependency will be downloaded. To avoid this you can install curl yourself using the standard methods for your platform.")
endif()

find_package(Threads REQUIRED)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

find_package(Boost REQUIRED COMPONENTS) 

include_directories(${Boost_INCLUDE_DIRS}) 

configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/config.json)

add_subdirectory(src)
add_subdirectory(lib)
add_subdirectory(test)
enable_testing()

add_test(NAME cryptobot_test COMMAND ${CMAKE_BINARY_DIR}/cryptobot_test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
