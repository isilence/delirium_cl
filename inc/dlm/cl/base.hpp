#ifndef DLM_CL_BASE_HPP_
#define DLM_CL_BASE_HPP_
#pragma once
#include "dlm/env/macro.h"

DLM_CMODULE_START
#ifdef __APPLE__
    #include <OpenCL/opencl.h>
#else
    #include <CL/cl.h>
    #include <CL/cl_ext.h>
#endif
DLM_CMODULE_END

#include <exception>

namespace dlmcl {

class CLException {};

// ======================================
//      device descriptors
// ======================================

enum MEMORY_TYPE
{
    MT_GENERIC  = 0x1,
    MT_HOST     = 0x2,
    MT_DEVICE   = 0x4
};

struct PCITopology
{
    int bus;
    int dev;
    int fn;
};

struct MemoryCap
{
    long    size;
    int     banks;
};

struct CacheInfo
{
    long size;
    long width;
};

struct MemoryInfo
{
    MemoryCap global;
    MemoryCap local;
    CacheInfo cache;

    int     supportedTypes;
    bool    isSMA;
};

struct ComputeInfo
{
    int units;
    int opwidth;
    int warp;
    int maxGroup;
    int maxDim;
};

struct DeviceInfo
{
    MemoryInfo  memory;
    ComputeInfo compute;
    PCITopology topology;

    cl_device_type      type;
};


} // ::dlmcl
#endif // DLM_CL_DEVICE_HPP_
