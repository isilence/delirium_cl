#ifndef DLM_CL_DEVICE_HPP_
#define DLM_CL_DEVICE_HPP_
#pragma once
#include "dlm/cl/base.hpp"

namespace dlmcl {

// ======================================
//      device descriptors
// ======================================

enum DEVICE_VENDOR
{
    CDV_UNKNOWN = 0,
    CDV_AMD,
    CDV_INTEL,
    CDV_NVIDIA
};

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
    enum DEVICE_VENDOR  vendor;
};


// ======================================
//      device information builder
// ======================================

class DeviceInfoBuilder
{
private:
    DeviceInfo deviceInfo;
    const cl_device_id dev;

    void fill(void) noexcept;
    #if !defined(DLM_CL_SKIP_DEVICE_AMD)
        void fillAMD(void) noexcept;
    #endif
    #if !defined(DLM_CL_SKIP_DEVICE_INTEL)
        void fillIntel(void) noexcept;
    #endif
    #if !defined(DLM_CL_SKIP_DEVICE_NVIDIA)
        void fillNvidia(void) noexcept;
    #endif

public:
    explicit DeviceInfoBuilder(cl_device_id dev);
    DeviceInfo get(void) noexcept;
};

inline DeviceInfoBuilder::DeviceInfoBuilder(cl_device_id dev)
    : dev(dev)
{
    fill();
}

inline DeviceInfo DeviceInfoBuilder::get(void) noexcept
{
    return deviceInfo;
}

// ======================================
//          Device class
// ======================================

class Device
{
private:
    void initialize(cl_device_id device);
    void release(void) noexcept;

public:
    explicit Device(cl_device_id device);
    explicit Device(Device&&)   = default;
    Device(void)                = delete;
    Device(const Device&)       = delete;
    ~Device(void)               noexcept;

    cl_device_id    device;
    cl_context      context;
    cl_platform_id  platform;
    DeviceInfo      info;
};

inline Device::Device(cl_device_id device)
{
    initialize(device);
}

inline Device::~Device(void) noexcept
{
    release();
}

} // ::dlmcl
#endif // DLM_CL_DEVICE_HPP_
