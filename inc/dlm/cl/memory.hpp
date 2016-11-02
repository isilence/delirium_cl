#ifndef DLM_CL_MEMORY_HPP_
#define DLM_CL_MEMORY_HPP_
#include <stddef.h>

#include "dlm/env/system.h"
#include "dlm/allocator.hpp"
#include "dlm/cl/base.hpp"
#include "dlm/cl/device.hpp"

namespace dlmcl {

static inline cl_mem_flags getMapType(cl_mem_flags accessType)
{
     switch (accessType) {
        case CL_MEM_READ_WRITE:
            return CL_MAP_READ | CL_MAP_WRITE;
        case CL_MEM_READ_ONLY:
            return CL_MAP_READ;
        case CL_MEM_WRITE_ONLY:
            return CL_MAP_WRITE;
        default:
            return 0;
    }
}

class Memory
{
protected:
    Device device;
    size_t memsize;
    cl_mem_flags accessType;

public:
    Memory(const Memory&) = delete;
    Memory(Device device, size_t size, cl_mem_flags accessType) :
        device(device),
        memsize(size),
        accessType(accessType)
    {
        // todo:
        if (accessType != CL_MEM_READ_WRITE &&
            accessType != CL_MEM_READ_ONLY &&
            accessType != CL_MEM_WRITE_ONLY)
            throw 1;
    }

    virtual ~Memory(void) {};

    virtual void*  switchToHost(void) = 0;
    virtual cl_mem switchToDevice(void) = 0;

    inline size_t getSize() const
    {
        return memsize;
    }

    inline cl_mem_flags getAccessType() const
    {
        return accessType;
    }
};

class GenericMemory : public Memory
{
    cl_mem devMemory;
    void* hostMemory;

public:
    GenericMemory(Device device, size_t size, cl_mem_flags accessType) :
        Memory(device, size, accessType)
    {
        // todo: check error, check hostmemory
        hostMemory = aligned_malloc(size, DLM_PAGE_SIZE);
        devMemory = clCreateBuffer(device.context, accessType, size, nullptr, NULL);
    }

    ~GenericMemory()
    {
        aligned_free(hostMemory);
        clReleaseMemObject(devMemory);
    }

    virtual void* switchToHost(void) override
    {
        if (accessType == CL_MEM_WRITE_ONLY || accessType == CL_MEM_READ_WRITE)
            clEnqueueReadBuffer(device.queue, devMemory, CL_TRUE, 0, memsize, hostMemory, 0, NULL, NULL);
        return hostMemory;
    };

    virtual cl_mem switchToDevice(void) override
    {
        if (accessType == CL_MEM_READ_ONLY || accessType == CL_MEM_READ_WRITE)
            clEnqueueWriteBuffer(device.queue, devMemory, CL_TRUE, 0, memsize, hostMemory, 0, NULL, NULL);
        return devMemory;
    };

};


class HostMemory : public Memory
{
    cl_mem devMemory;
    void* hostMemory;
    cl_mem_flags maptype;

public:
    HostMemory(Device device, size_t size, cl_mem_flags accessType) :
        Memory(device, size, accessType)
    {
        cl_int error;
        devMemory = clCreateBuffer(device.context, CL_MEM_ALLOC_HOST_PTR | accessType, size, nullptr, &error);
        maptype = getMapType(accessType);
    }

    ~HostMemory()
    {
        clReleaseMemObject(devMemory);
    }

    virtual void* switchToHost(void) override
    {
        hostMemory = clEnqueueMapBuffer(device.queue, devMemory, CL_TRUE, maptype, 0, memsize, 0, NULL, NULL, NULL);
        return hostMemory;
    };

    virtual cl_mem switchToDevice(void) override
    {
        clEnqueueUnmapMemObject(device.queue, devMemory, hostMemory, 0, NULL, NULL);
        return devMemory;
    };

};


class DeviceMemory : public Memory
{
    cl_mem devMemory;
    void* hostMemory;
    cl_mem_flags maptype;

public:
    DeviceMemory(Device device, size_t size, cl_mem_flags accessType) :
        Memory(device, size, accessType)
    {
        cl_int error;
        devMemory = clCreateBuffer(device.context, CL_MEM_USE_PERSISTENT_MEM_AMD | accessType, size, nullptr, &error);
        maptype = getMapType(accessType);
    }

    ~DeviceMemory()
    {
        clReleaseMemObject(devMemory);
    }

    virtual void* switchToHost(void) override
    {
        hostMemory = clEnqueueMapBuffer(device.queue, devMemory, CL_TRUE, maptype, 0, memsize, 0, NULL, NULL, NULL);
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
