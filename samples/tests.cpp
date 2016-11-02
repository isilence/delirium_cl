#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

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

double testCopy(dlmcl::Device dev, size_t memsize, dlmcl::Memory* mem)
{
    const int itCount = 1000;
    void* tmp = new char[mem->getSize()];
    clFinish(dev.queue);

    Timer t;
    for (int i=0; i<itCount; ++i) {
        void* buffer = mem->switchToHost();
        memcpy(tmp, buffer, mem->getSize());
        memset(buffer, 0xdeadbeef, mem->getSize());
        mem->switchToDevice();
    }
    clFinish(dev.queue);
    t.stop();

    delete [] tmp;
    return t.get();
}

double testSq(dlmcl::Device dev, dlmcl::Memory* in, dlmcl::Memory* out, size_t n)
{
    const int itCount = 200;
    size_t sz = n * sizeof(float);
    cl_int errcode;

    Program prg = buildProgram(dev, DLM_SAMPLE_DIR "simple.cl", "squareArray");
    char* tmp = new char[out->getSize()];

    const size_t locSz[] = {64ul};
    clFinish(dev.queue);
    Timer t;
    for (int it = 0; it < itCount; ++it) {
        void* inMem = in->switchToHost();
        memset(inMem, 0xdeadbeef, in->getSize());

        cl_mem devIn = in->switchToDevice();
        checkErrorEx(errcode = clSetKernelArg(prg.kernel, 0, sizeof(devIn), &devIn););

        cl_mem devOut = out->switchToDevice();
        checkErrorEx(errcode = clSetKernelArg(prg.kernel, 1, sizeof(devOut), &devOut););

        errcode = clEnqueueNDRangeKernel(dev.queue, prg.kernel, 1, NULL, &n, locSz, 0, NULL, NULL);
        checkError(clEnqueueNDRangeKernel);
        void* res = out->switchToHost();
        memcpy(tmp, res, out->getSize());
    }
    clFinish(dev.queue);
    t.stop();


    releaseProgram(prg);
    delete [] tmp;
    return t.get();
}
