cmake_minimum_required(VERSION 3.6)
project(delirium)

include(preferences.cmake)

set(DIR_ROOT ${PROJECT_SOURCE_DIR})
set(DIR_SRC ${DIR_ROOT}/src)
set(DIR_INC ${DIR_ROOT}/inc)
set(DIR_BIN ${DIR_ROOT}/bin)
set(DIR_SAMPLE ${DIR_ROOT}/samples)
set(DIR_UNIT_TESTS ${DIR_ROOT}/unittests)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIR_BIN})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIR_BIN})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DIR_BIN})

add_subdirectory(${DIR_SRC})
add_subdirectory(${DIR_UNIT_TESTS})
add_subdirectory(${DIR_SAMPLE})
