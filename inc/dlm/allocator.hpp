#ifndef DLM_ALLOCATOR_HPP_
#define DLM_ALLOCATOR_HPP_
#pragma once
#include <limits.h>

#ifdef _MSC_VER
    #include <stdlib.h>
    static inline void* aligned_malloc(size_t size, size_t align)
    {
        return _aligned_malloc(size, align);
    }

    static inline void aligned_free(void *ptr)
    {
        _aligned_free(ptr);
    }
#else
    #include <stdlib.h>
    static inline void* aligned_malloc(size_t size, size_t align) {
        void *result;
        if (posix_memalign(&result, align, size))
            result = 0;
        return result;
    }

    static inline void aligned_free(void *ptr) {
        free(ptr);
    }
#endif



#endif /* DLM_ALLOCATOR_HPP_ */
