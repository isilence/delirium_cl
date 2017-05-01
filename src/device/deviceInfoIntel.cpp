#if !defined(DLM_CL_SKIP_DEVICE_INTEL)
#include "dlm/env/macro.h"
DLM_CMODULE_START
#include "cl/cl_ext_intel.h"
DLM_CMODULE_END
#include "dlm/cl/device.hpp"
using namespace dlmcl;

void DeviceInfoBuilder::fillIntel(void) noexcept
{
    if (deviceInfo.type & CL_DEVICE_TYPE_GPU)
        deviceInfo.memory.isSMA = true;
}

#endif // DLM_CL_SKIP_DEVICE_AMD
