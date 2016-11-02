#ifndef DLM_CL_BASE_HPP_
#define DLM_CL_BASE_HPP_
#ifdef __APPLE__
    #include <OpenCL/opencl.h>
#else
    #include <CL/cl.h>
    #include <CL/cl_ext.h>
#endif

#endif // DLM_CL_DEVICE_HPP_
