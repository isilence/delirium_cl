#include "dlm/cl/memory.hpp"
using namespace dlmcl;

HostMemory::HostMemory(Device& device, size_t size, cl_mem_flags accessType) :
    Memory(device, size, accessType),
    isDeviceMode(true)
{
    cl_int error;
    deviceMemory = clCreateBuffer(device.context, CL_MEM_ALLOC_HOST_PTR | accessType, size, nullptr, &error);
    if (error != CL_SUCCESS)
        throw new CLException();

    hostMemory = nullptr;
    maptype = Memory::getMapType(accessType);
}

HostMemory::~HostMemory()
{
    clReleaseMemObject(deviceMemory);
}

void HostMemory::switchToHost(cl_command_queue queue)
{
    if (!isDeviceMode)
        return;
    isDeviceMode = false;

    cl_int error;
    void* hostSiteMemory = clEnqueueMapBuffer(queue, deviceMemory, CL_TRUE, maptype, 0, memsize, 0, NULL, NULL, &error);
    if (error != CL_SUCCESS)
        throw new CLException();

    hostMemory = hostSiteMemory;
};

void HostMemory::switchToDevice(cl_command_queue queue)
{
    if (isDeviceMode)
        return;
    isDeviceMode = true;

    cl_int error = clEnqueueUnmapMemObject(queue, deviceMemory, hostMemory, 0, NULL, NULL);
    if (error != CL_SUCCESS)
        throw new CLException();
    hostMemory = nullptr;
};

