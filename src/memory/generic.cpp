#include "dlm/cl/memory.hpp"
using namespace dlmcl;

GenericMemory::GenericMemory(Device& device, size_t size, cl_mem_flags accessType) :
    Memory(device, size, accessType)
{
    // todo: check error, check hostmemory
    hostMemory = aligned_malloc(size, DLM_PAGE_SIZE);
    devMemory = clCreateBuffer(device.context, accessType, size, nullptr, NULL);
}

GenericMemory::~GenericMemory(void)
{
    aligned_free(hostMemory);
    clReleaseMemObject(devMemory);
}

void* GenericMemory::switchToHost(void)
{
    if (accessType == CL_MEM_WRITE_ONLY || accessType == CL_MEM_READ_WRITE)
        clEnqueueReadBuffer(device.queue, devMemory, CL_TRUE, 0, memsize, hostMemory, 0, NULL, NULL);
    return hostMemory;
};

cl_mem GenericMemory::switchToDevice(void)
{
    if (accessType == CL_MEM_READ_ONLY || accessType == CL_MEM_READ_WRITE)
        clEnqueueWriteBuffer(device.queue, devMemory, CL_TRUE, 0, memsize, hostMemory, 0, NULL, NULL);
    return devMemory;
};
