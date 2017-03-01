#include "dlm/cl/device.hpp"
using namespace dlmcl;

cl_platform_id Device::getPlatform(cl_device_id clDevice)
{
    cl_platform_id clPlatform;
    const cl_int res = clGetDeviceInfo(clDevice, CL_DEVICE_PLATFORM, sizeof(clPlatform), &clPlatform, nullptr);
    if (res != CL_SUCCESS)
        throw new CLException();
    return clPlatform;
}

cl_context Device::createIsolatedContext(const cl_device_id clDevice)
{
    cl_int errcode;
    const cl_context context = clCreateContext(nullptr, 1, &clDevice, nullptr, nullptr, &errcode);
    if (context == NULL)
        throw CLException();
    return context;
}

void Device::initialize(cl_device_id clDevice)
{
    const cl_platform_id clPlatform = getPlatform(clDevice);
    const cl_context clContext = createIsolatedContext(clDevice);

    device = clDevice;
    platform = clPlatform;
    context = clContext;

    DeviceInfoFiller(info, device).fill();
}

void Device::release(void) noexcept
{
    clReleaseContext(context);
    clReleaseDevice(device);
}
