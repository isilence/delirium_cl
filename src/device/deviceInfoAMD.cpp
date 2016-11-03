#ifndef DLM_CL_SKIP_DEVICE_AMD
#include "dlm/env/macro.h"
DLM_CMODULE_START
#include "amd/cl_ext.h"
DLM_CMODULE_END
#include "dlm/cl/device.hpp"
#include "dlm/cl/deviceInfo.hpp"
using namespace dlmcl;

bool DeviceInfoAMD::isIGPU(cl_device_id clDevice)
{
    cl_device_topology_amd top;
    cl_int ret = clGetDeviceInfo(clDevice, CL_DEVICE_TOPOLOGY_AMD, sizeof(top), &top, NULL);
    if (ret != CL_SUCCESS)
        return false;
    return top.pcie.bus == 0;
}


bool DeviceInfoAMD::supportDeviceMemory(const cl_device_id DLM_UNUSED(device))
{
    return true;
}


#endif // DLM_CL_SKIP_DEVICE_AMD
