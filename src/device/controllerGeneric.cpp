#include <string.h>
#include <vector>
#include "dlm/cl/device.hpp"
#include "dlm/cl/controllers.hpp"
using namespace dlmcl;

// ======================================
//      generic filler helpers
// ======================================

static void setDefaultInfo(DeviceInfo& info) noexcept
{
    memset((void*)&info, 0, sizeof(info));
    info.topology.bus = -1;
}

static void setDeviceType(DeviceInfo& info, const cl_device_id device) noexcept
{
    cl_device_type type;
    const cl_int error = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, nullptr);
    info.type = (error == CL_SUCCESS) ? type : 0;
}

static void setMemoryInfo(DeviceInfo& info, const cl_device_id device) noexcept
{
    cl_int err;
    info.memory.supportedTypes = MT_GENERIC | MT_HOST;
    info.memory.isSMA = false;

    cl_ulong cacheSize;
    err = clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cacheSize), &cacheSize, nullptr);
    if (err == CL_SUCCESS)
        info.memory.cache.size = cacheSize;

    cl_uint laneWidth;
    err = clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(laneWidth), &laneWidth, nullptr);
    if (err == CL_SUCCESS)
        info.memory.cache.width = laneWidth;

    cl_ulong size;
    err = clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(size), &size, nullptr);
    if (err == CL_SUCCESS)
        info.memory.global.size = (size_t)size;

    err = clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(size), &size, nullptr);
    if (err == CL_SUCCESS)
        info.memory.local.size = size;
}

static void setComputeUnitInfo(DeviceInfo& info, const cl_device_id device) noexcept
{
    cl_int err;
    info.memory.supportedTypes = MT_GENERIC;

    cl_uint computeUniteNum;
    err = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUniteNum), &computeUniteNum, nullptr);
    if (err == CL_SUCCESS)
        info.compute.units = computeUniteNum;

    size_t maxGroupSize;
    err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxGroupSize), &maxGroupSize, nullptr);
    if (err == CL_SUCCESS)
        info.compute.maxGroup = (int)maxGroupSize;

    cl_uint maxDim;
    err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(maxDim), &maxDim, nullptr);
    if (err == CL_SUCCESS)
        info.compute.maxDim = (int)maxDim;
}

static void setMemoryArchitectureInfo(DeviceInfo &info, const cl_device_id device) noexcept
{
    cl_bool isUnified;
    cl_int err = clGetDeviceInfo(device, CL_DEVICE_HOST_UNIFIED_MEMORY, sizeof(isUnified), &isUnified, nullptr);
    if (err == CL_SUCCESS)
        info.memory.isSMA = (isUnified != 0);
}

// ======================================
//          impl
// ======================================

DeviceInfo GenericController::getInfo(cl_device_id device) noexcept
{
    DeviceInfo info;
    setDefaultInfo(info);
    setDeviceType(info, device);
    setMemoryInfo(info, device);
    setMemoryArchitectureInfo(info, device);
    setComputeUnitInfo(info, device);

    if (info.type & CL_DEVICE_TYPE_CPU)
        info.memory.isSMA = true;

    return info;
}

// ======================================
//          register default
// ======================================

void dlmcl::registerDefaultVendorControllers(void)
{
    static const char VENDOR_NAME_AMD1[] = "Advanced Micro Devices, Inc.";
    static const char VENDOR_NAME_AMD2[] = "AuthenticAMD";
    static const char VENDOR_NAME_INTEL1[] = "GenuineIntel";
    static const char VENDOR_NAME_INTEL2[] = "Intel(R) Corporation";
    static const char VENDOR_NAME_NVIDIA[] = "NVIDIA Corporation";

    Controller* def = new GenericController();
    Controller* amd = new AMDController();
    Controller* nvidia = new NVidiaController();
    Controller* intel = new IntelController();

    static std::vector<Controller*> guard = {def, amd, nvidia, intel};

    Device::registerVendorController(Device::DEFAULT_VENDOR, def);
    Device::registerVendorController(VENDOR_NAME_AMD1, amd);
    Device::registerVendorController(VENDOR_NAME_AMD2, amd);
    Device::registerVendorController(VENDOR_NAME_INTEL1, intel);
    Device::registerVendorController(VENDOR_NAME_INTEL2, intel);
    Device::registerVendorController(VENDOR_NAME_NVIDIA, nvidia);
}
