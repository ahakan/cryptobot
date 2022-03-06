cmake_minimum_required(VERSION 3.6.0)
project(elog)


# Add a library with the above sources
add_library(${PROJECT_NAME} src/elog.cpp)
add_library(sub::libelog ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/inc
)