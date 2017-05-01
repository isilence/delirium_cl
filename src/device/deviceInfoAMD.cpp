#if !defined(DLM_CL_SKIP_DEVICE_AMD)
#include "dlm/env/macro.h"
DLM_CMODULE_START
#include "cl/cl_ext_amd.h"
DLM_CMODULE_END
#include "dlm/cl/device.hpp"
using namespace dlmcl;

static void initTopology(DeviceInfo& di, const cl_device_id dev) noexcept
{
    cl_device_topology_amd top;
    const cl_int err = clGetDeviceInfo(dev, CL_DEVICE_TOPOLOGY_AMD, sizeof(top), &top, NULL);
    if (err == CL_SUCCESS) {
        di.topology.bus = top.pcie.bus;
        di.topology.dev = top.pcie.device;
        di.topology.fn = top.pcie.function;
    }
}

static void initMisc(DeviceInfo& di, const cl_device_id dev) noexcept
{
    cl_int err;

    cl_uint wavefront;
    err = clGetDeviceInfo(dev, CL_DEVICE_WAVEFRONT_WIDTH_AMD, sizeof(wavefront), &wavefront, NULL);
    if (err == CL_SUCCESS)
        di.compute.warp = wavefront;

    cl_uint bankNum;
    err = clGetDeviceInfo(dev, CL_DEVICE_LOCAL_MEM_BANKS_AMD, sizeof(bankNum), &bankNum, NULL);
    if (err == CL_SUCCESS)
        di.memory.local.banks = bankNum;

    err = clGetDeviceInfo(dev, CL_DEVICE_GLOBAL_MEM_CHANNEL_BANKS_AMD, sizeof(bankNum), &bankNum, NULL);
    if (err == CL_SUCCESS)
        di.memory.global.banks = bankNum;
}

void DeviceInfoBuilder::fillAMD(void) noexcept
{
    initTopology(deviceInfo, dev);
    initMisc(deviceInfo, dev);

    deviceInfo.memory.supportedTypes |= MT_DEVICE;

    const int CPU_MEM_BUS = 0;
    if (deviceInfo.topology.bus == CPU_MEM_BUS)
        deviceInfo.memory.isSMA = true;
}

#endif // DLM_CL_SKIP_DEVICE_AMD
