#include "dlm/cl/memobj.hpp"
using namespace dlmcl;

cl_mem_flags Memobj::getMapType(const cl_mem_flags accessType)
{
     switch (accessType) {
        case CL_MEM_READ_ONLY:
            return CL_MAP_READ;
        case CL_MEM_WRITE_ONLY:
            return CL_MAP_WRITE;
    }
    return CL_MAP_READ | CL_MAP_WRITE;
}

bool Memobj::isAccessTypeValid(const cl_mem_flags accessType)
{
    return  accessType == CL_MEM_READ_WRITE ||
            accessType == CL_MEM_READ_ONLY ||
            accessType == CL_MEM_WRITE_ONLY;
}

Memobj* Memobj::getOptimal(Device& dev, size_t size, cl_mem_flags clMemType)
{
    if (dev.info.mem.isSMA)
        return new HostMemory(dev, size, clMemType);

    switch (clMemType) {
        case CL_MEM_READ_ONLY:
            return new DeviceMemory(dev, size, clMemType);
        case CL_MEM_WRITE_ONLY:
        case CL_MEM_READ_WRITE:
            return new GenericMemory(dev, size, clMemType);
        default :
            throw new CLException();
    }
}
