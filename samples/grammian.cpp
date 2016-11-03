#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "utils.hpp"
#include "dlm/cl/memory.hpp"

using namespace dlmcl;


void gramCpu(   const float * __restrict const  vs,
                float * __restrict const        output,
                const int                       n,
                const int                       vecSz)
{
   for (int i=0; i<n; ++i)
   for (int j=0; j<n; ++j)
   {
       const float * const e1 = &vs[vecSz * i];
       const float * const e2 = &vs[vecSz * j];

       float s = 0.0f;
       for (int k=0; k<vecSz; ++k) {
           s += e1[k]*e2[k];
       }
       output[i*n + j] = s;
   }
}

void testc(Device& dev, size_t n, size_t k)
{
    float* in = new float[n*k];
    float* out = new float[n*n];
    for (size_t i=0; i<n*k; ++i)
        in[i] = (float)(rand() % 80);
    gramCpu(in, out, n, k);
    cl_int errcode;
    Program prg = buildProgram(dev, DLM_SAMPLE_DIR "grammian.cl", "computeGramMrx");

    const size_t glSz[] = {(size_t)n, (size_t)n};
    const size_t locSz[] = {8ul, 8ul};
    cl_int error;
    cl_command_queue queue = clCreateCommandQueueWithProperties(dev.context, dev.device, nullptr, &error);
    clFinish(queue);

    GenericMemory devIn(dev, sizeof(float)*n*k, CL_MEM_READ_ONLY);
    GenericMemory devOut(dev, sizeof(float)*n*n, CL_MEM_WRITE_ONLY);
    devIn.switchToHost(queue);
    void* inMem = devIn.getMemHost();
    memcpy(inMem, in, sizeof(float) * n * k);
    devIn.switchToDevice(queue);
    cl_mem devInm = devIn.getMemDevice();
    checkErrorEx(errcode = clSetKernelArg(prg.kernel, 0, sizeof(devInm), &devInm););

    devOut.switchToDevice(queue);
    cl_mem devOutm = devOut.getMemDevice();
    checkErrorEx(errcode = clSetKernelArg(prg.kernel, 1, sizeof(devOutm), &devOutm););
    checkErrorEx(errcode = clSetKernelArg(prg.kernel, 2, sizeof(k), &k););

    errcode = clEnqueueNDRangeKernel(queue, prg.kernel, 2, NULL, glSz, locSz, 0, NULL, NULL);
    checkError(clEnqueueNDRangeKernel);
    clFinish(queue);

    devOut.switchToHost(queue);
    float* res = (float*)devOut.getMemHost();
    if (!compare(out, res, n))
        std::cout << "result mismatch!" << std::endl;

    clReleaseCommandQueue(queue);
    releaseProgram(prg);
}

double testGrammian(dlmcl::Device& dev, dlmcl::Memory* in, dlmcl::Memory* out, size_t vecSz, size_t n)
{
    const int itCount = 50;
	const int inSz = vecSz * n * sizeof(float);
	const int outSz = n * n * sizeof(float);
    cl_int errcode;

    Program prg = buildProgram(dev, DLM_SAMPLE_DIR "grammian.cl", "computeGramMrx");
    char* tmp = new char[out->getSize()];
    cl_int error;
    cl_command_queue queue = clCreateCommandQueueWithProperties(dev.context, dev.device, nullptr, &error);
    const size_t glSz[] = {(size_t)n, (size_t)n};
    const size_t locSz[] = {8ul, 8ul};
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
        cl_mem devOut = out->getMemDevice();;
        checkErrorEx(errcode = clSetKernelArg(prg.kernel, 1, sizeof(devOut), &devOut););
        checkErrorEx(errcode = clSetKernelArg(prg.kernel, 2, sizeof(vecSz), &vecSz););

        errcode = clEnqueueNDRangeKernel(queue, prg.kernel, 2, NULL, glSz, locSz, 0, NULL, NULL);
        checkError(clEnqueueNDRangeKernel);
        out->switchToHost(queue);
        //memcpy(tmp, res, out->getSize());
    }
    clFinish(queue);
    t.stop();

    testc(dev, n, vecSz);

    clReleaseCommandQueue(queue);
    releaseProgram(prg);
    delete [] tmp;
    return t.get() / itCount;
}
