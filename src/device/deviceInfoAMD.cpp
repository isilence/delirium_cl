#ifndef DLM_CL_SKIP_DEVICE_AMD
#include "dlm/env/macro.h"
DLM_CMODULE_START
#include "amd/cl_ext.h"
DLM_CMODULE_END
#include "dlm/cl/device.hpp"
#include "dlm/cl/deviceInfo.hpp"
using namespace dlmcl;

bool DeviceInfoAMD::isSupportDeviceMemory(const cl_device_id DLM_UNUSED(device))
{
    return true;
}

void DeviceInfoAMD::initDeviceInfo(const cl_device_id device, DeviceInfo& deviceInfo)
{
    cl_int error;

    if (deviceInfo.type == DT_GPU) {
        cl_device_topology_amd top;
        error = clGetDeviceInfo(device, CL_DEVICE_TOPOLOGY_AMD, sizeof(top), &top, NULL);
        if (error == CL_SUCCESS && top.pcie.bus == 0)
            deviceInfo.type = DT_IGPU;
    }

    cl_uint wavefront;
    error = clGetDeviceInfo(device, CL_DEVICE_WAVEFRONT_WIDTH_AMD, sizeof(wavefront), &wavefront, NULL);
    if (error == CL_SUCCESS)
        deviceInfo.executionWidth = wavefront;

    cl_uint locBankCnt;
    error = clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_BANKS_AMD, sizeof(locBankCnt), &locBankCnt, NULL);
    if (error == CL_SUCCESS)
        deviceInfo.localMemoryBanks = locBankCnt;

    cl_uint globBankCnt;
    const cl_int ret = clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_CHANNEL_BANKS_AMD, sizeof(globBankCnt), &globBankCnt, NULL);
    if (ret == CL_SUCCESS)
        deviceInfo.globalMemoryBanks = globBankCnt;

}

#endif // DLM_CL_SKIP_DEVICE_AMD
