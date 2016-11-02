cmake_minimum_required(VERSION 3.6)
project(ocl_lib)

set(DIR_ROOT ${PROJECT_SOURCE_DIR})
set(DIR_SRC ${DIR_ROOT}/src)
set(DIR_INC ${DIR_ROOT}/inc)
set(DIR_BIN ${DIR_ROOT}/bin)
set(DIR_SAMPLE ${DIR_ROOT}/samples)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIR_BIN})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIR_BIN})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DIR_BIN})

#add_executable(ocl_lib ${SOURCE_FILES})
add_subdirectory(${DIR_SAMPLE})
