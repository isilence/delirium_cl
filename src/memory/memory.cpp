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
