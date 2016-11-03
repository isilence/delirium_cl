#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "utils.hpp"
#include "dlm/cl/memory.hpp"

using namespace dlmcl;

bool compare(  const float* __restrict const xs,
            const float* __restrict const ys,
            const int n)
{
    for (int i=0; i<n*n; ++i) {
        if (fabs(xs[i] - ys[i]) > 0.01) {
            std::cout << i << " " << fabs(xs[i] - ys[i]) << std::endl;
            return false;
        }
    }
    return true;
}

double testCopy(dlmcl::Device& dev, dlmcl::Memory* mem)
{
    const int itCount = 300;
    void* tmp = new char[mem->getSize()];

    cl_int error;
    cl_command_queue queue = clCreateCommandQueueWithProperties(dev.context, dev.device, nullptr, &error);

    Timer t;
    for (int i=0; i<itCount; ++i) {
        mem->switchToHost(queue);
        void* buffer = mem->getMemHost();
        memcpy(tmp, buffer, mem->getSize());
        memset(buffer, (unsigned int)0xdeadbeef, mem->getSize()); //-V575
        mem->switchToDevice(queue);
    }
    clFinish(queue);
    t.stop();

    clReleaseCommandQueue(queue);
    delete [] tmp;
    return t.get() / itCount;
}

double testSq(dlmcl::Device& dev, dlmcl::Memory* in, dlmcl::Memory* out, size_t n)
{
    const int itCount = 200;
    cl_int errcode;

    Program prg = buildProgram(dev, DLM_SAMPLE_DIR "simple.cl", "squareArray");
    char* tmp = new char[out->getSize()];
    cl_int error;
    cl_command_queue queue = clCreateCommandQueueWithProperties(dev.context, dev.device, nullptr, &error);

    const size_t locSz[] = {64ul};
    clFinish(queue);
    Timer t;
    for (int it = 0; it < itCount; ++it) {
        in->switchToHost(queue);
        void* inMem = in->getMemHost();
        memset(inMem, (unsigned int)0xdeadbeef, in->getSize()); //-V575

        in->switchToDevice(queue);
        cl_mem devIn = in->getMemDevice();
        checkErrorEx(errcode = clSetKernelArg(prg.kernel, 0, sizeof(devIn), &devIn););

        out->switchToDevice(queue);
        cl_mem devOut = out->getMemDevice();
        checkErrorEx(errcode = clSetKernelArg(prg.kernel, 1, sizeof(devOut), &devOut););

        errcode = clEnqueueNDRangeKernel(queue, prg.kernel, 1, NULL, &n, locSz, 0, NULL, NULL);
        checkError(clEnqueueNDRangeKernel);
        out->switchToHost(queue);
        void* res = out->getMemHost();
        memcpy(tmp, res, out->getSize());
    }
    clFinish(queue);
    t.stop();

    clReleaseCommandQueue(queue);
    releaseProgram(prg);
    delete [] tmp;
    return t.get() / itCount;
}
