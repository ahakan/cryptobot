cmake_minimum_required(VERSION 3.6.0)
project(libhttp)


# Add a library with the above sources
add_library(${PROJECT_NAME} INTERFACE)
add_library(sub::libhttp ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    INTERFACE ${PROJECT_SOURCE_DIR}/inc
)