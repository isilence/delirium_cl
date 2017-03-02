#ifndef DLM_CL_MEMOBJ_POOL_HPP_
#define DLM_CL_MEMOBJ_POOL_HPP_
#pragma once
#include <map>
#include "dlm/cl/memobj.hpp"

namespace dlmcl {

class MemobjPool
{
protected:
    std::map<void*, Memobj*> map;

    void* allocateMemory(Device& device, const size_t size, const cl_mem_flags accessType);

public:
    MemobjPool() = default;
    MemobjPool(MemobjPool&&) = default;
    MemobjPool(const MemobjPool&) = delete;
    ~MemobjPool() = default;

    template<typename T = void>
    inline T* alloc(Device& device, const size_t size, const cl_mem_flags accessType)
    {
        return (T*)allocateMemory(device, size * sizeof(T), accessType);
    }
    bool free(void* const ptr);
    bool sync(void* const ptr, cl_command_queue queue);
    Memobj* get(void* const ptr);
};

} // ::dlmcl
#endif // DLM_CL_MEMOBJ_POOL_HPP_
