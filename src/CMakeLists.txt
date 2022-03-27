include_directories(${CMAKE_SOURCE_DIR}/inc)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

set(SOURCES
  ${SOURCE_DIR}/sql.cpp
  ${SOURCE_DIR}/opel.cpp
  ${SOURCE_DIR}/utilities.cpp
  ${SOURCE_DIR}/websocket.cpp
  ${SOURCE_DIR}/server.cpp
  ${SOURCE_DIR}/client.cpp
  ${SOURCE_DIR}/main.cpp

)

add_executable(cryptobot ${SOURCES}) 

target_link_libraries(cryptobot sub::libelog sub::libjson sub::libhttp -lcap ${SQLite3_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} ${CURL_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
