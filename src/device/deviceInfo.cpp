#include <string.h>
#include "dlm/cl/deviceInfo.hpp"

using namespace dlmcl;

static const char VENDOR_NAME_AMD1[] = "Advanced Micro Devices, Inc.";
static const char VENDOR_NAME_AMD2[] = "AuthenticAMD";
static const char VENDOR_NAME_INTEL1[] = "GenuineIntel";
static const char VENDOR_NAME_INTEL2[] = "Intel(R) Corporation";

void DeviceInfo::initDeviceVendor(void)
{
    char buffer[50];
    vendor = CDV_UNKNOWN;

    const cl_int error = clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(buffer), buffer, nullptr);
    if (error != CL_SUCCESS)
        return;
    if (strcmp(buffer, VENDOR_NAME_AMD1) == 0 ||
        strcmp(buffer, VENDOR_NAME_AMD2) == 0)
    {
        vendor = CDV_AMD;
        return;
    }

    if (strcmp(buffer, VENDOR_NAME_INTEL1) == 0 ||
        strcmp(buffer, VENDOR_NAME_INTEL2) == 0)
    {
        vendor = CDV_INTEL;
        return;
    }
}

void DeviceInfo::initDeviceType(void)
{
    type = DT_UNKNOWN;

    cl_device_type clDeviceType;
    cl_int error = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(clDeviceType), &clDeviceType, nullptr);
    if (error != CL_SUCCESS)
        return;

    if (clDeviceType == CL_DEVICE_TYPE_CPU)
        type = DT_CPU;
    else if (clDeviceType != CL_DEVICE_TYPE_GPU && clDeviceType != CL_DEVICE_TYPE_ACCELERATOR)
        type = DT_GPU;
}

bool DeviceInfo::supportMemoryType(enum DLM_MEMORY_TYPE memType) const
{
    if (memType == MT_DEVICE) {
        #if !defined(DLM_CL_SKIP_DEVICE_AMD)
            if (vendor == CDV_AMD)
                return DeviceInfoAMD::isSupportDeviceMemory(device);
        #endif
        return false;
    }
    return true;
}

void DeviceInfo::setDefault(void)
{
    vendor = CDV_UNKNOWN;
    type = DT_UNKNOWN;

    executionWidth = 0;
    globalMemoryBanks = 0;
    localMemoryBanks = 0;
}


void DeviceInfo::initialize(cl_device_id clDevice)
{
    device = clDevice;

    setDefault();
    initDeviceVendor();
    initDeviceType();

    switch (vendor) {
    #if !defined(DLM_CL_SKIP_DEVICE_AMD)
        case CDV_AMD:
            DeviceInfoAMD::initDeviceInfo(clDevice, *this);
            break;
    #endif
        case CDV_UNKNOWN:
        default:
            break;
    }
}


