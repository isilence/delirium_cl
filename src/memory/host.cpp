#include "dlm/cl/memory.hpp"
using namespace dlmcl;

HostMemory::HostMemory(Device& device, size_t size, cl_mem_flags accessType) :
        Memory(device, size, accessType),
        hostMemory(nullptr)
{
    cl_int error;
    devMemory = clCreateBuffer(device.context, CL_MEM_ALLOC_HOST_PTR | accessType, size, nullptr, &error);
    maptype = Memory::getMapType(accessType);
}

HostMemory::~HostMemory()
{
    clReleaseMemObject(devMemory);
}

void* HostMemory::switchToHost(cl_command_queue queue)
{
    hostMemory = clEnqueueMapBuffer(queue, devMemory, CL_TRUE, maptype, 0, memsize, 0, NULL, NULL, NULL);
    return hostMemory;
};

cl_mem HostMemory::switchToDevice(cl_command_queue queue)
{
    clEnqueueUnmapMemObject(queue, devMemory, hostMemory, 0, NULL, NULL);
    return devMemory;
};
