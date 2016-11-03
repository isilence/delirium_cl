#ifndef DLM_CL_BASE_HPP_
#define DLM_CL_BASE_HPP_
#include "dlm/env/macro.h"

DLM_CMODULE_START
#ifdef __APPLE__
    #include <OpenCL/opencl.h>
#else
    #include <CL/cl.h>
    #include <CL/cl_ext.h>
#endif
DLM_CMODULE_END

#include <exception>

namespace dlmcl {

class CLException {};

}; // ::dlmcl
#endif // DLM_CL_DEVICE_HPP_
