cmake_minimum_required(VERSION 3.6)
project(benchmark)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

include_directories(${DIR_INC})

add_definitions(-DDLM_SAMPLE_DIR="${DIR_SAMPLE}/")

set(SOURCE_FILES
    ${DIR_SAMPLE}/benchmark/main.cpp

    ${DIR_SAMPLE}/utils/benchmark.hpp
    ${DIR_SAMPLE}/utils/device.cpp
    ${DIR_SAMPLE}/utils/program.cpp
    ${DIR_SAMPLE}/utils/tasks.hpp
    ${DIR_SAMPLE}/utils/timer.hpp
    ${DIR_SAMPLE}/utils/utils.hpp
)

add_executable(benchmark ${SOURCE_FILES})
target_link_libraries (benchmark ${OpenCL_LIBRARY})
target_link_libraries (benchmark dlmCl)


