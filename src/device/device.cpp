#include "dlm/cl/device.hpp"
using namespace dlmcl;

cl_platform_id Device::getPlatform(cl_device_id clDevice)
{
    cl_platform_id clPlatform;
    cl_int res = clGetDeviceInfo(clDevice, CL_DEVICE_PLATFORM, sizeof(clPlatform), &clPlatform, nullptr);
    if (res != CL_SUCCESS)
        throw new CLException();
    return clPlatform;
}

cl_context Device::createSinleContext(cl_device_id clDevice)
{
    cl_int errcode;
    cl_context context = clCreateContext(nullptr, 1, &clDevice, nullptr, nullptr, &errcode);
    if (context == NULL)
        throw CLException();
    return context;
}

void Device::initialize(cl_device_id clDevice)
{
    cl_platform_id clPlatform = getPlatform(clDevice);
    cl_context clContext = createSinleContext(clDevice);

    device = clDevice;
    platform = clPlatform;
    context = clContext;
}

void Device::release(void)
{
    clReleaseContext(context);
    clReleaseDevice(device);
}
