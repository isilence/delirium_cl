#if !defined(DLM_CL_SKIP_VENDOR_INTEL)
#include "dlm/env/macro.h"
DLM_CMODULE_START
#include "cl/cl_ext_intel.h"
DLM_CMODULE_END
#include "dlm/cl/device.hpp"
#include "dlm/cl/controllers.hpp"
using namespace dlmcl;

DeviceInfo IntelController::getInfo(cl_device_id device) noexcept
{
    DeviceInfo info = GenericController::getInfo(device);
    if (info.type & CL_DEVICE_TYPE_GPU)
        info.memory.isSMA = true;

    return info;
}

#endif // DLM_CL_SKIP_VENDOR_INTEL
