set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/inc)
set(TEST_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/test)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

include_directories(${INCLUDE_DIR})
include_directories(${TEST_INCLUDE_DIR})

set(SOURCES 
    test_utilities.cc
    ${SOURCE_DIR}/sql.cpp
    ${SOURCE_DIR}/opel.cpp
    ${SOURCE_DIR}/utilities.cpp
    ${SOURCE_DIR}/websocket.cpp
    ${SOURCE_DIR}/server.cpp
    ${SOURCE_DIR}/client.cpp
)


add_executable(cryptobot_test test_main.cc ${SOURCES})

target_link_libraries(cryptobot_test sub::libgmock sub::libgtest sub::libelog sub::libjson sub::libhttp -lcap ${SQLite3_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} ${CURL_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
