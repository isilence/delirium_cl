#ifndef DLM_CL_DEVICE_HPP_
#define DLM_CL_DEVICE_HPP_
#include "dlm/cl/base.hpp"

namespace dlmcl {

struct Device
{
    cl_device_id device;
    cl_context context;
    cl_platform_id platform;
    cl_command_queue queue;
};



}; // ::dlmcl
#endif // DLM_CL_DEVICE_HPP_
