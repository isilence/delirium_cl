#ifndef INIT_HPP_
#define INIT_HPP_
#include <string>
#include "dlm/cl/device.hpp"
#include "dlm/cl/memobj.hpp"
#include "timer.hpp"

#define checkError(func) \
  if (errcode != CL_SUCCESS)\
  {\
    printf("Error in " #func "\nError code = %d\n", errcode);\
    throw 1;\
  }
#define checkErrorEx(command) \
  command; \
  checkError(command);

void printDeviceInfo(dlmcl::Device& device);

struct Program {
    cl_kernel kernel;
    cl_program program;
};

void releaseProgram(Program p);
Program buildProgram(dlmcl::Device& dev, std::string path, std::string kernelName);

#endif // INIT_HPP_
