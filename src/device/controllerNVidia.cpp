#if !defined(DLM_CL_SKIP_VENDOR_NVIDIA)
#include "dlm/env/macro.h"
DLM_CMODULE_START
#include "cl/cl_ext_nvidia.h"
DLM_CMODULE_END
#include "dlm/cl/device.hpp"
#include "dlm/cl/controllers.hpp"
using namespace dlmcl;

DeviceInfo NVidiaController::getInfo(cl_device_id device) noexcept
{
    DeviceInfo info = GenericController::getInfo(device);
    cl_int err;

    cl_uint warp;
    err = clGetDeviceInfo(device, CL_DEVICE_WARP_SIZE_NV, sizeof(warp), &warp, NULL);
    if (err == CL_SUCCESS)
        info.compute.warp = warp;

    cl_bool isSMA;
    err = clGetDeviceInfo(device, CL_DEVICE_INTEGRATED_MEMORY_NV, sizeof(isSMA), &isSMA, NULL);
    if (err == CL_SUCCESS)
        info.memory.isSMA = (isSMA != 0);

    return info;
}

#endif // DLM_CL_SKIP_VENDOR_NVIDIA
