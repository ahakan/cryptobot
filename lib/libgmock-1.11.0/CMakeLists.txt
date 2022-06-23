cmake_minimum_required(VERSION 3.6.0)
project(gmock)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/gmock-all.cc)

add_library(sub::libgmock ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)