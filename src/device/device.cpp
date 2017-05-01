#include "dlm/cl/device.hpp"
using namespace dlmcl;

// ======================================
//          helpers
// ======================================

static cl_platform_id getPlatform(cl_device_id clDevice)
{
    cl_platform_id clPlatform;
    const cl_int res = clGetDeviceInfo(clDevice, CL_DEVICE_PLATFORM, sizeof(clPlatform), &clPlatform, nullptr);

    if (res != CL_SUCCESS)
        throw new CLException();
    return clPlatform;
}

static cl_context createIsolatedContext(const cl_device_id clDevice)
{
    cl_int res;
    const cl_context context = clCreateContext(nullptr, 1, &clDevice, nullptr, nullptr, &res);

    if (res != CL_SUCCESS)
        throw CLException();
    return context;
}

// ======================================
//          device impl
// ======================================

void Device::initialize(cl_device_id clDevice)
{
    cl_platform_id clPlatform = getPlatform(clDevice);
    cl_context clContext = createIsolatedContext(clDevice);

    device = clDevice;
    platform = clPlatform;
    context = clContext;
    info = DeviceInfoBuilder(device).get();
}

void Device::release(void) noexcept
{
    clReleaseContext(context);
    clReleaseDevice(device);
}
