#include "dlm/env/macro.h"
DLM_CMODULE_START
#include "amd/cl_ext.h"
DLM_CMODULE_END
#include "dlm/cl/memory.hpp"
using namespace dlmcl;

DeviceMemory::DeviceMemory(Device& device, size_t size, cl_mem_flags type) :
    HostMemory(device)
{
    cl_int error;
    deviceMemory = clCreateBuffer(device.context, CL_MEM_USE_PERSISTENT_MEM_AMD | type, size, nullptr, &error);
    if (error != CL_SUCCESS)
        throw new CLException();

    memsize = size;
    accessType = type;
    hostMemory = nullptr;
    maptype = Memory::getMapType(type);
}

