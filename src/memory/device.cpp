#include "dlm/cl/memory.hpp"
using namespace dlmcl;

DeviceMemory::DeviceMemory(Device& device, size_t size, cl_mem_flags accessType) :
    Memory(device, size, accessType),
    hostMemory(nullptr)
{
    cl_int error;
    devMemory = clCreateBuffer(device.context, CL_MEM_USE_PERSISTENT_MEM_AMD | accessType, size, nullptr, &error);
    maptype = Memory::getMapType(accessType);
}

DeviceMemory::~DeviceMemory()
{
    clReleaseMemObject(devMemory);
}

void* DeviceMemory::switchToHost(cl_command_queue queue)
{
    hostMemory = clEnqueueMapBuffer(queue, devMemory, CL_TRUE, maptype, 0, memsize, 0, NULL, NULL, NULL);
    return hostMemory;
};

cl_mem DeviceMemory::switchToDevice(cl_command_queue queue)
{
    clEnqueueUnmapMemObject(queue, devMemory, hostMemory, 0, NULL, NULL);
    return devMemory;
};
