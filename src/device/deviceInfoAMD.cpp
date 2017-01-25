#if !defined(DLM_CL_SKIP_DEVICE_AMD)
#include "dlm/env/macro.h"
DLM_CMODULE_START
#include "cl/amd_cl_ext.h"
DLM_CMODULE_END
#include "dlm/cl/deviceInfo.hpp"
using namespace dlmcl;

static void initTopology(DeviceInfo& di, const cl_device_id dev) noexcept
{
    cl_device_topology_amd top;
    cl_int err = clGetDeviceInfo(dev, CL_DEVICE_TOPOLOGY_AMD, sizeof(top), &top, NULL);
    if (err == CL_SUCCESS) {
        di.top.bus = top.pcie.bus;
        di.top.dev = top.pcie.device;
        di.top.fn = top.pcie.function;
    }
}

static void initMisc(DeviceInfo& di, const cl_device_id dev) noexcept
{
    cl_int err;

    cl_uint wavefront;
    err = clGetDeviceInfo(dev, CL_DEVICE_WAVEFRONT_WIDTH_AMD, sizeof(wavefront), &wavefront, NULL);
    if (err == CL_SUCCESS)
        di.comp.waveFront = wavefront;

    cl_uint memBankNum;
    err = clGetDeviceInfo(dev, CL_DEVICE_LOCAL_MEM_BANKS_AMD, sizeof(memBankNum), &memBankNum, NULL);
    if (err == CL_SUCCESS)
        di.mem.local.bankNum = memBankNum;

    err = clGetDeviceInfo(dev, CL_DEVICE_GLOBAL_MEM_CHANNEL_BANKS_AMD, sizeof(memBankNum), &memBankNum, NULL);
    if (err == CL_SUCCESS)
        di.mem.global.bankNum = memBankNum;
}

void DeviceInfoFiller::fillAMD(void)
{
    initTopology(di, dev);
    initMisc(di, dev);

    di.mem.supportedTypes |= MT_DEVICE;

    const int CPU_MEM_BUS = 0;
    if (di.top.bus == CPU_MEM_BUS)
        di.mem.isSMA = true;
}

#endif // DLM_CL_SKIP_DEVICE_AMD
