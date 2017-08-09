#if !defined(DLM_CL_SKIP_VENDOR_AMD)
#include "dlm/env/macro.h"
DLM_CMODULE_START
#include "cl/cl_ext_amd.h"
DLM_CMODULE_END
#include "dlm/cl/device.hpp"
#include "dlm/cl/controllers.hpp"
using namespace dlmcl;

static void initTopology(DeviceInfo& info, const cl_device_id device) noexcept
{
    cl_device_topology_amd top;
    const cl_int err = clGetDeviceInfo(device, CL_DEVICE_TOPOLOGY_AMD, sizeof(top), &top, NULL);
    if (err == CL_SUCCESS) {
        info.topology.bus = top.pcie.bus;
        info.topology.dev = top.pcie.device;
        info.topology.fn = top.pcie.function;
    }
}

static void initMisc(DeviceInfo& info, const cl_device_id device) noexcept
{
    cl_int err;

    cl_uint wavefront;
    err = clGetDeviceInfo(device, CL_DEVICE_WAVEFRONT_WIDTH_AMD, sizeof(wavefront), &wavefront, NULL);
    if (err == CL_SUCCESS)
        info.compute.warp = wavefront;

    cl_uint bankNum;
    err = clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_BANKS_AMD, sizeof(bankNum), &bankNum, NULL);
    if (err == CL_SUCCESS)
        info.memory.local.banks = bankNum;

    err = clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_CHANNEL_BANKS_AMD, sizeof(bankNum), &bankNum, NULL);
    if (err == CL_SUCCESS)
        info.memory.global.banks = bankNum;
}

DeviceInfo AMDController::getInfo(cl_device_id device) noexcept
{
    DeviceInfo info = GenericController::getInfo(device);
    initTopology(info, device);
    initMisc(info, device);
    info.memory.supportedTypes |= MT_DEVICE;

    const int CPU_MEM_BUS = 0;
    if (info.topology.bus == CPU_MEM_BUS)
        info.memory.isSMA = true;

    return info;
}

const char* AMDController::getCompilationOptions(cl_device_id device, enum OPTIMIZATION_LEVEL level) noexcept
{
    static const char opts[] = "-cl-fast-relaxed-math -cl-no-signed-zeros -cl-mad-enable -O4 -D DLM_AMD";
    return opts;
}

#endif // DLM_CL_SKIP_VENDOR_AMD
