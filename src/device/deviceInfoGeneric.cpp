#include <string.h>
#include "dlm/cl/deviceInfo.hpp"

using namespace dlmcl;

static void setDefaultInfo(DeviceInfo& di) noexcept
{
    memset((void*)&di, 0, sizeof(di));
    di.top.bus = -1;
}

static void setDeviceVendor(DeviceInfo& di, const cl_device_id dev) noexcept
{
    static const char VENDOR_NAME_AMD1[] = "Advanced Micro Devices, Inc.";
    static const char VENDOR_NAME_AMD2[] = "AuthenticAMD";
    static const char VENDOR_NAME_INTEL1[] = "GenuineIntel";
    static const char VENDOR_NAME_INTEL2[] = "Intel(R) Corporation";

    char buffer[50];
    di.vendor = CDV_UNKNOWN;

    const cl_int error = clGetDeviceInfo(dev, CL_DEVICE_VENDOR, sizeof(buffer), buffer, nullptr);
    if (error != CL_SUCCESS)
        return;
    if (strcmp(buffer, VENDOR_NAME_AMD1) == 0 ||
        strcmp(buffer, VENDOR_NAME_AMD2) == 0)
    {
        di.vendor = CDV_AMD;
        return;
    }

    if (strcmp(buffer, VENDOR_NAME_INTEL1) == 0 ||
        strcmp(buffer, VENDOR_NAME_INTEL2) == 0)
    {
        di.vendor = CDV_INTEL;
        return;
    }
}

static void setDeviceType(DeviceInfo& di, const cl_device_id dev) noexcept
{
    cl_int error = clGetDeviceInfo(dev, CL_DEVICE_TYPE, sizeof(di.type), &di.type, nullptr);
    if (error != CL_SUCCESS)
        di.type = 0;
}

static void setMemoryInfo(DeviceInfo& di, const cl_device_id dev) noexcept
{
    cl_int err;
    di.mem.supportedTypes = MT_GENERIC | MT_HOST;
    di.mem.isSMA = false;

    cl_ulong cacheSize;
    err = clGetDeviceInfo(dev, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cacheSize), &cacheSize, nullptr);
    if (err == CL_SUCCESS)
        di.mem.cacheSize = cacheSize;

    cl_uint laneWidth;
    err = clGetDeviceInfo(dev, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(laneWidth), &laneWidth, nullptr);
    if (err == CL_SUCCESS)
        di.mem.cacheLaneWidth = laneWidth;

    cl_ulong size;
    err = clGetDeviceInfo(dev, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(size), &size, nullptr);
    if (err == CL_SUCCESS)
        di.mem.global.size = (size_t)size;
    err = clGetDeviceInfo(dev, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(size), &size, nullptr);
    if (err == CL_SUCCESS)
        di.mem.local.size = size;
}

static void setComputeUnitInfo(DeviceInfo& di, const cl_device_id dev) noexcept
{
    cl_int err;
    di.mem.supportedTypes = MT_GENERIC;

    cl_uint cuNum;
    err = clGetDeviceInfo(dev, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cuNum), &cuNum, nullptr);
    if (err == CL_SUCCESS)
        di.comp.computeUnitNum = cuNum;

    size_t maxGroupSize;
    err = clGetDeviceInfo(dev, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxGroupSize), &maxGroupSize, nullptr);
    if (err == CL_SUCCESS)
        di.comp.maxGroup = maxGroupSize;

    cl_uint maxDim;
    err = clGetDeviceInfo(dev, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(maxDim), &maxDim, nullptr);
    if (err == CL_SUCCESS)
        di.comp.maxDim = maxDim;
}


void DeviceInfoFiller::fill(void)
{
    setDefaultInfo(di);
    setDeviceVendor(di, dev);
    setDeviceType(di, dev);
    setMemoryInfo(di, dev);
    setComputeUnitInfo(di, dev);

    if (di.type & CL_DEVICE_TYPE_CPU)
        di.mem.isSMA = true;

    #if !defined(DLM_CL_SKIP_DEVICE_AMD)
        if (di.vendor == CDV_AMD)
            fillAMD();
    #endif
    #if !defined(DLM_CL_SKIP_DEVICE_INTEL)
        if (di.vendor == CDV_INTEL)
            fillIntel();
    #endif
}
