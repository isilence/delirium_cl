#ifndef INIT_HPP_
#define INIT_HPP_
#include <string>
#include "dlm/cl/device.hpp"
#include "dlm/cl/memory.hpp"
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

void initOpenCl(dlmcl::Device& dev, int devIdx);
void releaseOpenCl(dlmcl::Device dev);
void printDeviceInfo(dlmcl::Device device);

struct Program {
    cl_kernel kernel;
    cl_program program;
};

void releaseProgram(Program p);
Program buildProgram(dlmcl::Device dev, std::string path, std::string kernelName);


bool compare(  const float* __restrict const xs,
            const float* __restrict const ys,
            const int n);
double testCopy(dlmcl::Device dev, size_t memsize, dlmcl::Memory* mem);
double testGrammian(dlmcl::Device dev, dlmcl::Memory* in, dlmcl::Memory* out, size_t vecSz, size_t n);
double testSq(dlmcl::Device dev, dlmcl::Memory* in, dlmcl::Memory* out, size_t n);

#endif // INIT_HPP_
