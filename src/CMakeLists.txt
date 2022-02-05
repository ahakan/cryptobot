include_directories(${CMAKE_SOURCE_DIR}/inc)

add_library(libjson SHARED IMPORTED)
set_target_properties(libjson PROPERTIES
  IMPORTED_LOCATION 
    "${CMAKE_SOURCE_DIR}/lib/libjson-1.9.5/lib/libjsoncpp.a"
  INTERFACE_INCLUDE_DIRECTORIES 
    "${CMAKE_SOURCE_DIR}/lib/libjson-1.9.5/include/json"
)

add_library(libwebsockets SHARED IMPORTED)
set_target_properties(libwebsockets PROPERTIES
  IMPORTED_LOCATION 
    "${CMAKE_SOURCE_DIR}/lib/libwebsockets-4.3.0/lib/libwebsockets.a"
  INTERFACE_INCLUDE_DIRECTORIES 
    "${CMAKE_SOURCE_DIR}/lib/libwebsockets-4.3.0/include"
)

add_library(libelog SHARED IMPORTED)
set_target_properties(libelog PROPERTIES
  IMPORTED_LOCATION 
    "${CMAKE_SOURCE_DIR}/lib/libelog-1.0.0/src/libelog.a"
  INTERFACE_INCLUDE_DIRECTORIES 
    "${CMAKE_SOURCE_DIR}/lib/libelog-1.0.0/inc"
)

add_library(libsqlite SHARED IMPORTED)
set_target_properties(libsqlite PROPERTIES
  IMPORTED_LOCATION 
    "${CMAKE_SOURCE_DIR}/lib/libsqlite-3.37.2/lib/libsqlite.a"
  INTERFACE_INCLUDE_DIRECTORIES 
    "${CMAKE_SOURCE_DIR}/lib/libsqlite-3.37.2/inc"
)


find_package(Threads REQUIRED)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS) # header only libraries must not be added here
include_directories(${Boost_INCLUDE_DIRS}) 

set(SOURCE_DIR
    
  ${CMAKE_SOURCE_DIR}/src
)

set(SOURCES

  ${SOURCE_DIR}/main.cpp
  ${SOURCE_DIR}/sql.cpp
  ${SOURCE_DIR}/utilities.cpp
  ${SOURCE_DIR}/websocket.cpp
)

add_executable(cryptobot ${SOURCES}) 
target_link_libraries(cryptobot libjson libwebsockets libelog libsqlite -lssl -lcrypto -lcap ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
