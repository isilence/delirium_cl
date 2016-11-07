#include "dlm/cl/memory.hpp"
using namespace dlmcl;

cl_mem_flags Memory::getMapType(const cl_mem_flags accessType)
{
     switch (accessType) {
        case CL_MEM_READ_ONLY:
            return CL_MAP_READ;
        case CL_MEM_WRITE_ONLY:
            return CL_MAP_WRITE;
    }
    return CL_MAP_READ | CL_MAP_WRITE;
}

bool Memory::isAccessTypeValid(const cl_mem_flags accessType)
{
    return  accessType == CL_MEM_READ_WRITE ||
            accessType == CL_MEM_READ_ONLY ||
            accessType == CL_MEM_WRITE_ONLY;
}

Memory* Memory::getOptimal(Device& dev, size_t size, cl_mem_flags clMemType)
{
    if (dev.info.type == DT_CPU || dev.info.type == DT_IGPU) {
        //if (clMemType == CL_MEM_READ_ONLY && dev.info.supportMemoryType(MT_DEVICE))
        //    return new DeviceMemory(dev, size, clMemType);

        return new HostMemory(dev, size, clMemType);
    }

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
