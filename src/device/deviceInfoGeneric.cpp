#include <string.h>
#include "dlm/cl/device.hpp"

using namespace dlmcl;

static void setDefaultInfo(DeviceInfo& info) noexcept
{
    memset((void*)&info, 0, sizeof(info));
    info.topology.bus = -1;
}

static void setDeviceVendor(DeviceInfo& info, const cl_device_id dev) noexcept
{
    const int MAX_VENDOR_NAME = 50;

    static const char VENDOR_NAME_AMD1[] = "Advanced Micro Devices, Inc.";
    static const char VENDOR_NAME_AMD2[] = "AuthenticAMD";
    static const char VENDOR_NAME_INTEL1[] = "GenuineIntel";
    static const char VENDOR_NAME_INTEL2[] = "Intel(R) Corporation";
    static const char VENDOR_NAME_NVIDIA[] = "NVIDIA Corporation";

    char buffer[MAX_VENDOR_NAME];
    info.vendor = CDV_UNKNOWN;

    const cl_int error = clGetDeviceInfo(dev, CL_DEVICE_VENDOR, sizeof(buffer), buffer, nullptr);
    if (error != CL_SUCCESS)
        return;

    if (strcmp(buffer, VENDOR_NAME_AMD1) == 0 || strcmp(buffer, VENDOR_NAME_AMD2) == 0) {
        info.vendor = CDV_AMD;
    } else if (strcmp(buffer, VENDOR_NAME_INTEL1) == 0 || strcmp(buffer, VENDOR_NAME_INTEL2) == 0) {
        info.vendor = CDV_INTEL;
    } else if (strcmp(buffer, VENDOR_NAME_NVIDIA) == 0) {
        info.vendor = CDV_NVIDIA;
    }
}

static void setDeviceType(DeviceInfo& info, const cl_device_id dev) noexcept
{
    cl_device_type type;
    const cl_int error = clGetDeviceInfo(dev, CL_DEVICE_TYPE, sizeof(type), &type, nullptr);
    info.type = (error == CL_SUCCESS) ? type : 0;
}

static void setMemoryInfo(DeviceInfo& info, const cl_device_id dev) noexcept
{
    cl_int err;
    info.memory.supportedTypes = MT_GENERIC | MT_HOST;
    info.memory.isSMA = false;

    cl_ulong cacheSize;
    err = clGetDeviceInfo(dev, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cacheSize), &cacheSize, nullptr);
    if (err == CL_SUCCESS)
        info.memory.cache.size = cacheSize;

    cl_uint laneWidth;
    err = clGetDeviceInfo(dev, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(laneWidth), &laneWidth, nullptr);
    if (err == CL_SUCCESS)
        info.memory.cache.width = laneWidth;

    cl_ulong size;
    err = clGetDeviceInfo(dev, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(size), &size, nullptr);
    if (err == CL_SUCCESS)
        info.memory.global.size = (size_t)size;

    err = clGetDeviceInfo(dev, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(size), &size, nullptr);
    if (err == CL_SUCCESS)
        info.memory.local.size = size;
}

static void setComputeUnitInfo(DeviceInfo& info, const cl_device_id dev) noexcept
{
    cl_int err;
    info.memory.supportedTypes = MT_GENERIC;

    cl_uint computeUniteNum;
    err = clGetDeviceInfo(dev, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUniteNum), &computeUniteNum, nullptr);
    if (err == CL_SUCCESS)
        info.compute.units = computeUniteNum;

    size_t maxGroupSize;
    err = clGetDeviceInfo(dev, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxGroupSize), &maxGroupSize, nullptr);
    if (err == CL_SUCCESS)
        info.compute.maxGroup = (int)maxGroupSize;

    cl_uint maxDim;
    err = clGetDeviceInfo(dev, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(maxDim), &maxDim, nullptr);
    if (err == CL_SUCCESS)
        info.compute.maxDim = (int)maxDim;
}

void DeviceInfoBuilder::fill(void) noexcept
{
    setDefaultInfo(deviceInfo);
    setDeviceVendor(deviceInfo, dev);
    setDeviceType(deviceInfo, dev);
    setMemoryInfo(deviceInfo, dev);
    setComputeUnitInfo(deviceInfo, dev);

    if (deviceInfo.type & CL_DEVICE_TYPE_CPU)
        deviceInfo.memory.isSMA = true;

    #if !defined(DLM_CL_SKIP_DEVICE_AMD)
        if (deviceInfo.vendor == CDV_AMD)
            fillAMD();
    #endif
    #if !defined(DLM_CL_SKIP_DEVICE_INTEL)
        if (deviceInfo.vendor == CDV_INTEL)
            fillIntel();
    #endif
    #if !defined(DLM_CL_SKIP_DEVICE_INTEL)
        if (deviceInfo.vendor == CDV_NVIDIA)
            fillNvidia();
    #endif
}
