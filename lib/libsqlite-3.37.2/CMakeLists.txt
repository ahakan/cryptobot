cmake_minimum_required(VERSION 3.6.0)
project(sqlite)


# Add a library with the above sources
add_library(${PROJECT_NAME} src/sqlite3.c)
add_library(sub::libsqlite ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/inc
)