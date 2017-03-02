#ifndef DLM_CL_KERNEL_HPP_
#define DLM_CL_KERNEL_HPP_
#pragma once
#include "dlm/cl/base.hpp"
#include "dlm/cl/memobjPool.hpp"

namespace dlmcl {

static inline cl_int setKernelArguments(cl_command_queue DLM_UNUSED(queue),
                                        cl_kernel DLM_UNUSED(kernel),
                                        size_t DLM_UNUSED(num))
{
    return CL_SUCCESS;
}

template <typename T, typename ...Args>
static inline cl_int setKernelArguments(cl_command_queue queue,
                                        cl_kernel kernel,
                                        cl_uint num,
                                        T param,
                                        Args... xs)
{
    const cl_int error = clSetKernelArg(kernel, num, sizeof(param), (void*)&param);
    if (error != CL_SUCCESS)
        return error;
    return setKernelArguments(queue, kernel, num+1, xs...);
}

template <typename ...Args>
static inline cl_int setKernelArguments(cl_command_queue queue,
                                        cl_kernel kernel,
                                        cl_uint num,
                                        Memobj* mem,
                                        Args... xs)
{
    mem->switchContext(queue, Memobj::MCT_DEVICE);
    mem->switchToDevice(queue);
    cl_mem clMem = mem->getDeviceMemory();
    return setKernelArguments(queue, kernel, num, clMem, xs...);
}

template <typename ...Args>
static inline cl_int setKernelArguments(cl_command_queue queue,
                                        cl_kernel kernel,
                                        cl_uint num,
                                        MemobjPool& pool,
                                        Memobj* mem,
                                        Args... xs)
{
    mem->switchContext(queue, Memobj::MCT_DEVICE);
    mem->switchToDevice(queue);
    cl_mem clMem = mem->getDeviceMemory();
    return setKernelArguments(queue, kernel, num, pool, clMem, xs...);
}

template <typename T, typename ...Args>
static inline cl_int setKernelArguments(cl_command_queue queue,
                                        cl_kernel kernel,
                                        cl_uint num,
                                        MemobjPool& pool,
                                        T* param,
                                        Args... xs)
{
    Memobj* const ptr = pool.get((void*)param);
    if (ptr == nullptr)
        return CL_MEM_OBJECT_ALLOCATION_FAILURE;

    return setKernelArguments(queue, kernel, num, pool, ptr, xs...);
}


template<typename ...Args>
static inline cl_int runKernel( cl_kernel kernel,
                                cl_command_queue queue,
                                const cl_uint dimSize,
                                const size_t globalSize[],
                                const size_t localSize[],
                                Args... xs)
{
    cl_int error = setKernelArguments(queue, kernel, 0, xs...);
    if (error != CL_SUCCESS)
        return error;
    return clEnqueueNDRangeKernel(queue, kernel, dimSize, NULL, globalSize, localSize, 0, NULL, NULL);
}

template<typename ...Args>
static inline cl_int runKernel( cl_kernel kernel,
                                cl_command_queue queue,
                                const cl_uint dimSize,
                                const size_t globalSize[],
                                const size_t localSize[],
                                MemobjPool& pool,
                                Args... xs)
{
    cl_int error = setKernelArguments(queue, kernel, 0, pool, xs...);
    if (error != CL_SUCCESS)
        return error;
    return clEnqueueNDRangeKernel(queue, kernel, dimSize, NULL, globalSize, localSize, 0, NULL, NULL);
}

} // ::dlmcl
#endif // DLM_CL_KERNEL_HPP_
