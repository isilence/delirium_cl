#ifndef DLM_ALLOCATOR_HPP_
#define DLM_ALLOCATOR_HPP_
#include <limits.h>

#ifdef _MSC_VER
    #include <stdlib.h>
    inline void* aligned_malloc(size_t size, size_t align)
    {
        return _aligned_malloc(size, align);
    }

    inline void aligned_free(void *ptr)
    {
        _aligned_free(ptr);
    }

#endif

//inline void* aligned_malloc(size_t size, size_t align) {
//    void *result;
//    #ifdef _MSC_VER
//        result = _aligned_malloc(size, align);
//    #else
//        if (posix_memalign(&result, align, size))
//            result = 0;
//    #endif
//    return result;
//}
//
//inline void aligned_free(void *ptr) {
//    #ifdef _MSC_VER
//        _aligned_free(ptr);
//    #else
//        free(ptr);
//    #endif
//}

#endif /* DLM_ALLOCATOR_HPP_ */
