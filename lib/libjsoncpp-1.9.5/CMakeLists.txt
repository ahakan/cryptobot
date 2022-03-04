cmake_minimum_required(VERSION 3.6.0)
project(libjson)


# Add a library with the above sources
add_library(${PROJECT_NAME} src/json_reader.cpp src/json_value.cpp src/json_writer.cpp)
add_library(sub::libjson ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/inc
)