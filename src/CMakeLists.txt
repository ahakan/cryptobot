include_directories(${CMAKE_SOURCE_DIR}/inc)

find_package(Threads REQUIRED)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS) # header only libraries must not be added here
include_directories(${Boost_INCLUDE_DIRS}) 


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
target_link_libraries(cryptobot sub::libelog sub::libjson sub::libsqlite -lssl -lcrypto -lcap -lcurl ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
