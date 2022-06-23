cmake_minimum_required(VERSION 3.6.0)
project(gtest)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/gtest-all.cc)

add_library(sub::libgtest ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)