set(BINARY ${CMAKE_PROJECT_NAME}_test)

include_directories(${CMAKE_SOURCE_DIR}/inc)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

set(SOURCES 
    ${TEST_SOURCES}
)

add_executable(${BINARY} ${TEST_SOURCES})

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} sub::libgtest sub::libelog)