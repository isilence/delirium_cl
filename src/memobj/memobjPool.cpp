#include "dlm/cl/memobjPool.hpp"
#include "dlm/allocator.hpp"

using namespace dlmcl;

typedef typename std::map<void*, Memobj*>::const_iterator TIter;

void* MemobjPool::allocateMemory(Device& device, const size_t sizeBytes, const cl_mem_flags accessType)
{
    Memobj* const mem = Memobj::getOptimal(device, sizeBytes, accessType);
    void* const hostPtr = mem->getHostMemory();

    map[hostPtr] = mem;
    return hostPtr;
}

bool MemobjPool::free(void* const ptr)
{
    TIter iter = map.find(ptr);
    if (iter != map.end()) {
        delete iter->second;
        map.erase(iter);
        return true;
    }
    return false;
}

Memobj* MemobjPool::get(void* const ptr)
{
    TIter iter = map.find(ptr);
    return (iter == map.end()) ? nullptr : iter->second;
}

bool MemobjPool::sync(void* const ptr, cl_command_queue queue)
{
    Memobj* const mem = get(ptr);
    if (mem == nullptr)
        return false;
    mem->switchToHost(queue);
    return true;
}
