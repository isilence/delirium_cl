#include "dlm/cl/memobj.hpp"
using namespace dlmcl;

GenericMemory::GenericMemory(   Device& device,
                                const size_t size,
                                const cl_mem_flags accessType) :
    Memobj(device, size, accessType),
    isDevice(true)
{
    cl_int err;
    void* const hostSiteMem = aligned_malloc(size, DLM_PAGE_SIZE);
    if (hostSiteMem == nullptr)
        throw new CLException();

    cl_mem devSiteMem = clCreateBuffer(device.context, accessType, size, nullptr, &err);
    if (err != CL_SUCCESS) {
        aligned_free(hostSiteMem);
        throw new CLException();
    }

    hostMemory = hostSiteMem;
    deviceMemory = devSiteMem;
}

GenericMemory::~GenericMemory(void)
{
    if (hostMemory != nullptr)
        aligned_free(hostMemory);
    hostMemory = nullptr;
    clReleaseMemObject(deviceMemory);
}

void GenericMemory::switchToHost(cl_command_queue queue)
{
    if (!isDevice)
        return;
    isDevice = false;

    if (accessType == CL_MEM_WRITE_ONLY || accessType == CL_MEM_READ_WRITE)
        clEnqueueReadBuffer(queue, deviceMemory, CL_TRUE, 0, memsize, hostMemory, 0, NULL, NULL);
}

void GenericMemory::switchToDevice(cl_command_queue queue)
{
    if (isDevice)
        return;
    isDevice = true;

    if (accessType == CL_MEM_READ_ONLY || accessType == CL_MEM_READ_WRITE)
        clEnqueueWriteBuffer(queue, deviceMemory, CL_TRUE, 0, memsize, hostMemory, 0, NULL, NULL);
}
