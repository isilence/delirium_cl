#ifndef DLM_CL_MEMORY_HPP_
#define DLM_CL_MEMORY_HPP_
#include <stddef.h>

#include "dlm/env/system.h"
#include "dlm/allocator.hpp"
#include "dlm/cl/base.hpp"
#include "dlm/cl/device.hpp"
#include "dlm/cl/deviceInfo.hpp"

namespace dlmcl {



class Memory
{
protected:
    Device& device;
    size_t memsize;
    cl_mem_flags accessType;

    static cl_mem_flags getMapType(const cl_mem_flags accessType);
    static bool isAccessTypeValid(const cl_mem_flags accessType);

public:
    Memory(const Memory&) = delete;
    Memory(Device& device, size_t size, cl_mem_flags accessType) :
        device(device),
        memsize(size),
        accessType(accessType)
    {
        if (!isAccessTypeValid(accessType))
            throw new CLException();
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
    GenericMemory(Device& device, size_t size, cl_mem_flags accessType);
    virtual ~GenericMemory();

    virtual void* switchToHost(void) override;
    virtual cl_mem switchToDevice(void) override;
};


class HostMemory : public Memory
{
    cl_mem devMemory;
    void* hostMemory;
    cl_mem_flags maptype;

public:
    HostMemory(Device& device, size_t size, cl_mem_flags accessType);
    virtual ~HostMemory();

    virtual void* switchToHost(void) override;

    virtual cl_mem switchToDevice(void) override;
};


class DeviceMemory : public Memory
{
    cl_mem devMemory;
    void* hostMemory;
    cl_mem_flags maptype;

public:
    DeviceMemory(Device& device, size_t size, cl_mem_flags accessType);
    virtual ~DeviceMemory();

    virtual void* switchToHost(void) override;
    virtual cl_mem switchToDevice(void) override;

};


inline Memory* getOptimalMemory(Device& dev, size_t size, cl_mem_flags accessType)
{
    if (dev.info.type == DT_CPU || dev.info.type == DT_IGPU) {
        return new HostMemory(dev, size, accessType);
    }

    switch (accessType) {
        case CL_MEM_READ_ONLY:
            return new DeviceMemory(dev, size, accessType);
        case CL_MEM_WRITE_ONLY:
            return new HostMemory(dev, size, accessType);
        case CL_MEM_READ_WRITE:
            return new GenericMemory(dev, size, accessType);
        default :
            throw -1;
    }
}


}; // ::dlmcl
#endif // DLM_CL_MEMORY_HPP_
