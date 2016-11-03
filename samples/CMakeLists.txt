cmake_minimum_required(VERSION 3.6)
project(sample)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

include_directories(${DIR_INC})

add_definitions(-DDLM_SAMPLE_DIR="${DIR_SAMPLE}/")

set(SOURCE_FILES
    device.cpp
    main.cpp
    program.cpp
    tests.cpp
    grammian.cpp
    timer.hpp
    utils.hpp
)

add_executable(sample ${SOURCE_FILES})
target_link_libraries (sample ${OpenCL_LIBRARY})
target_link_libraries (sample dlmCl)


