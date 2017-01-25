#include "dlm/env/macro.h"
DLM_CMODULE_START
#include "cl/amd_cl_ext.h"
DLM_CMODULE_END
#include "dlm/cl/memobj.hpp"
using namespace dlmcl;

DeviceMemory::DeviceMemory(Device& device, size_t size, cl_mem_flags type) :
    HostMemory(device)
{
    cl_int err;
    deviceMemory = clCreateBuffer(device.context, CL_MEM_USE_PERSISTENT_MEM_AMD | type, size, nullptr, &err);
    if (err != CL_SUCCESS)
        throw new CLException();

    memsize = size;
    accessType = type;
    hostMemory = nullptr;
    maptype = Memobj::getMapType(type);
}

