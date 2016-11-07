cmake_minimum_required(VERSION 3.6)
project(dlmCl)

find_package(OpenCL REQUIRED)
if (${OpenCL_VERSION_STRING} VERSION_LESS "1.2")
    message(FATAL_ERROR "OpenCl v1.2 or greater required, current is v${OpenCL_VERSION_STRING}")
endif()

include_directories(${DIR_INC})
include_directories(${OpenCL_INCLUDE_DIRS})

set(SRC_DLM_ENV
    ${DIR_INC}/dlm/env/macro.h
    ${DIR_INC}/dlm/env/system.h
    ${DIR_INC}/dlm/allocator.hpp
)
set(SRC_DLM_CL_MEMORY
    ${DIR_SRC}/memory/generic.cpp
    ${DIR_SRC}/memory/host.cpp
    ${DIR_SRC}/memory/device.cpp
    ${DIR_SRC}/memory/memory.cpp
)
set(SRC_DLM_CL_DEVICE
    ${DIR_SRC}/device/deviceInfo.cpp
    ${DIR_SRC}/device/deviceInfoAMD.cpp
    ${DIR_SRC}/device/device.cpp
)
set (SRC_DLM_CL_BASE
    ${DIR_INC}/dlm/cl/base.hpp
    ${DIR_INC}/dlm/cl/device.hpp
    ${DIR_INC}/dlm/cl/deviceInfo.hpp
    ${DIR_INC}/dlm/cl/memory.hpp
    ${DIR_INC}/dlm/cl/kernel.hpp
)

source_group("dlm\\cl" FILES ${SRC_DLM_CL_BASE})
source_group("dlm\\cl\\dev" FILES ${SRC_DLM_CL_DEVICE})
source_group("dlm\\cl\\mem" FILES ${SRC_DLM_CL_MEMORY})
source_group("dlm\\env" FILES ${SRC_DLM_ENV})

set(SOURCE_FILES
    ${SRC_DLM_ENV}
    ${SRC_DLM_CL_BASE}
    ${SRC_DLM_CL_DEVICE}
    ${SRC_DLM_CL_MEMORY}
)

add_library(dlmCl ${SOURCE_FILES})
link_directories(${OpenCL_LIBRARY})
