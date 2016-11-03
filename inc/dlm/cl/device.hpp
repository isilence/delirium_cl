#ifndef DLM_CL_DEVICE_HPP_
#define DLM_CL_DEVICE_HPP_
#include "dlm/cl/base.hpp"
#include "dlm/cl/deviceInfo.hpp"

namespace dlmcl {

class Device
{
protected:
    static cl_platform_id getPlatform(cl_device_id clDevice);
    static cl_context createSinleContext(cl_device_id clDevice);

    void initialize(cl_device_id device);
    void release(void);

public:
    Device() = delete;
    Device(const Device&) = delete;

    Device(const Device&& dev): info(dev.device) {
        device = dev.device;
        context = dev.context;
        platform = dev.platform;
    }
    Device(cl_device_id device): info(device)
    {
        initialize(device);
    }
    ~Device(void)
    {
        release();
    }

    cl_device_id device;
    cl_context context;
    cl_platform_id platform;
    const DeviceInfo info;
};



}; // ::dlmcl
#endif // DLM_CL_DEVICE_HPP_
