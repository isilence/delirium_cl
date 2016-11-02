#ifndef DLM_CL_MEMORY_HPP_
#define DLM_CL_MEMORY_HPP_
#include <stddef.h>

#include "dlm/env/system.h"
#include "dlm/allocator.hpp"
#include "dlm/cl/base.hpp"
#include "dlm/cl/device.hpp"

namespace dlmcl {

class Memory
{
protected:
    Device device;
    size_t memsize;

public:
    Memory(const Memory&) = delete;
    Memory(Device device, size_t size) :
        device(device),
        memsize(size)
    {}

    virtual ~Memory(void) {};

    virtual void*  switchToHost(void) = 0;
    virtual cl_mem switchToDevice(void) = 0;

    inline size_t getSize() const
    {
        return memsize;
    }
};

class GenericMemory : public Memory
{
    cl_mem devMemory;
    void* hostMemory;
    bool isGPUState;

    void initMemory(size_t size)
    {
        // todo: check error, check hostmemory
        hostMemory = aligned_malloc(size, DLM_PAGE_SIZE);
        devMemory = clCreateBuffer(device.context, CL_MEM_READ_WRITE, size, nullptr, NULL);
    }

    void releaseMemory(void)
    {
        aligned_free(hostMemory);
        clReleaseMemObject(devMemory);
    }

public:
    GenericMemory(Device device, size_t size) :
        Memory(device, size),
        isGPUState(false)
    {
        initMemory(size);
    }

    ~GenericMemory()
    {
        clReleaseMemObject(devMemory);
    }

    virtual void* switchToHost(void) override
    {
        return hostMemory;
    };

    virtual cl_mem switchToDevice(void) override
    {
        clEnqueueWriteBuffer(device.queue, devMemory, CL_TRUE, 0, memsize, hostMemory, 0, NULL, NULL);
        return devMemory;
    };

};


class HostMemobj : public Memory
{
    cl_mem devMemory;
    void* hostMemory;

    void initMemory(size_t size)
    {
        cl_int error;
        devMemory = clCreateBuffer(device.context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_WRITE, size, nullptr, &error);
    }

    void releaseMemory(void)
    {
        clReleaseMemObject(devMemory);
    }

public:
    HostMemobj(Device device, size_t size) :
        Memory(device, size)
    {
        initMemory(size);
    }

    ~HostMemobj()
    {
        releaseMemory();
    }

    virtual void* switchToHost(void) override
    {
        hostMemory = clEnqueueMapBuffer(device.queue, devMemory, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, memsize, 0, NULL, NULL, NULL);
        return hostMemory;
    };

    virtual cl_mem switchToDevice(void) override
    {
        clEnqueueUnmapMemObject(device.queue, devMemory, hostMemory, 0, NULL, NULL);
        return devMemory;
    };

};


}; // ::dlmcl
#endif // DLM_CL_MEMORY_HPP_
