cmake_minimum_required(VERSION 3.6)
project(sample)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

include_directories(${DIR_INC})
# message(111 "${DIR_INC}")

add_definitions(-DDLM_SAMPLE_DIR="${DIR_SAMPLE}/")

set(SOURCE_FILES
    ${DIR_INC}/dlm/env/macro.h
    ${DIR_INC}/dlm/env/system.h
    ${DIR_INC}/dlm/allocator.hpp
    ${DIR_INC}/dlm/cl/base.hpp
    ${DIR_INC}/dlm/cl/device.hpp
    ${DIR_INC}/dlm/cl/memory.hpp

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
