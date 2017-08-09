#if !defined(DLM_CL_SKIP_VENDOR_NVIDIA)
#include "dlm/env/macro.h"
DLM_CMODULE_START
#include "cl/cl_ext_nvidia.h"
DLM_CMODULE_END
#include "dlm/cl/device.hpp"
#include "dlm/cl/controllers.hpp"
using namespace dlmcl;


static void getPCITopology(DeviceInfo &info, cl_device_id device) noexcept
{
    // undocumented Nvidia API
    #define CL_DEVICE_PCI_BUS_ID_NV  0x4008
    #define CL_DEVICE_PCI_SLOT_ID_NV 0x4009

    cl_int bus = -1;
    cl_int slot = -1;

    cl_int err = clGetDeviceInfo (device, CL_DEVICE_PCI_BUS_ID_NV, sizeof(bus), &bus, NULL);
    if (err != CL_SUCCESS)
        return;

    err = clGetDeviceInfo(device, CL_DEVICE_PCI_SLOT_ID_NV, sizeof(slot), &slot, NULL);
    if (err != CL_SUCCESS)
        return;

    info.topology.bus = bus;
    info.topology.dev = (slot >> 3) & 0xff;
    info.topology.fn = slot & 0x7;
}

static void getMemoryArchitecture(DeviceInfo &info, cl_device_id device) noexcept
{
    cl_bool isSMA;
    const cl_int err = clGetDeviceInfo(device, CL_DEVICE_INTEGRATED_MEMORY_NV, sizeof(isSMA), &isSMA, NULL);
    if (err == CL_SUCCESS)
        info.memory.isSMA = (isSMA != 0);
}

static void getMemoryCaps(DeviceInfo &info, cl_device_id device) noexcept
{
    cl_uint warp;
    const cl_int err = clGetDeviceInfo(device, CL_DEVICE_WARP_SIZE_NV, sizeof(warp), &warp, NULL);
    if (err == CL_SUCCESS)
        info.compute.warp = warp;
}

DeviceInfo NvidiaController::getInfo(cl_device_id device) noexcept
{
    DeviceInfo info = GenericController::getInfo(device);
    getPCITopology(info, device);
    getMemoryArchitecture(info, device);
    getMemoryCaps(info, device);

    return info;
}

const char* NvidiaController::getCompilationOptions(cl_device_id device, enum OPTIMIZATION_LEVEL level) noexcept
{
    static const char opts[] = "-cl-fast-relaxed-math -cl-no-signed-zeros -cl-mad-enable -D DLM_NVIDIA";
    return opts;
}

#endif // DLM_CL_SKIP_VENDOR_NVIDIA
