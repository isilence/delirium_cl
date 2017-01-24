#ifndef DLM_CL_KERNEL_HPP_
#define DLM_CL_KERNEL_HPP_
#include "dlm/cl/base.hpp"

namespace dlmcl {

struct Void {};

template <typename T, typename ...Args>
inline
cl_int setKernelArguments(  cl_command_queue queue,
                            cl_kernel kernel,
                            size_t num,
                            T param,
                            Args... xs)
{
    const cl_int error = clSetKernelArg( kernel, num, sizeof(param), (void*)&param);
    if (error != CL_SUCCESS)
        return error;
    return setKernelArguments(queue, kernel, num+1, xs...);
}

template <typename ...Args>
inline
cl_int setKernelArguments(  cl_command_queue queue,
                            cl_kernel kernel,
                            size_t num,
                            Memory* mem,
                            Args... xs)
{
    mem->switchContext(queue, Memory::MCT_DEVICE);
    cl_mem clMem = mem->getDeviceMemory();
    return setKernelArguments(queue, kernel, num, clMem, xs...);
}

template <>
inline
cl_int setKernelArguments<Void>(cl_command_queue DLM_UNUSED(queue),
                                cl_kernel DLM_UNUSED(kernel),
                                size_t DLM_UNUSED(num),
                                Void DLM_UNUSED(stub))
{
    return CL_SUCCESS;
}

template<typename ...Args>
inline
cl_int runKernel(   cl_kernel kernel,
                    cl_command_queue queue,
                    const size_t dimSize,
                    const size_t globalSize[],
                    const size_t localSize[],
                    Args... xs)
{
    cl_int error = setKernelArguments(queue, kernel, 0, xs..., Void());
    if (error != CL_SUCCESS)
        return error;
    return clEnqueueNDRangeKernel(queue, kernel, dimSize, NULL, globalSize, localSize, 0, NULL, NULL);
}

} // ::dlmcl
#endif // DLM_CL_KERNEL_HPP_
