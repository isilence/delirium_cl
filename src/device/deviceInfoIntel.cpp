#if !defined(DLM_CL_SKIP_DEVICE_INTEL)
#include "dlm/env/macro.h"
DLM_CMODULE_START
#include "cl/intel_cl_ext.h"
DLM_CMODULE_END
#include "dlm/cl/deviceInfo.hpp"
using namespace dlmcl;

void DeviceInfoFiller::fillIntel(void)
{
    if (di.type & CL_DEVICE_TYPE_GPU)
        di.mem.isSMA = true;
}

#endif // DLM_CL_SKIP_DEVICE_AMD
