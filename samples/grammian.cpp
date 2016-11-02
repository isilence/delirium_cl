#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "utils.hpp"
#include "dlm/cl/memory.hpp"

using namespace dlmcl;

void gramCpu(   const float * __restrict const  vs,
                float * __restrict const        output,
                const int                       n,
                const int                       vecSz)
{
   clock_t startCPU = clock();

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

void testc(Device dev, size_t n, size_t k)
{
    float* in = new float[n*k];
    float* out = new float[n*n];
    for (int i=0; i<n*k; ++i)
        in[i] = rand() % 80;
    gramCpu(in, out, n, k);
    cl_int errcode;
    Program prg = buildProgram(dev, DLM_SAMPLE_DIR "grammian.cl", "computeGramMrx");

    const size_t glSz[] = {(size_t)n, (size_t)n};
    const size_t locSz[] = {8ul, 8ul};
    clFinish(dev.queue);

    GenericMemory devIn(dev, sizeof(float)*n*k, CL_MEM_READ_ONLY);
    GenericMemory devOut(dev, sizeof(float)*n*n, CL_MEM_WRITE_ONLY);
    void* inMem = devIn.switchToHost();
    memcpy(inMem, in, sizeof(float) * n * k);
    cl_mem devInm = devIn.switchToDevice();
    checkErrorEx(errcode = clSetKernelArg(prg.kernel, 0, sizeof(devInm), &devInm););
    cl_mem devOutm = devOut.switchToDevice();
    checkErrorEx(errcode = clSetKernelArg(prg.kernel, 1, sizeof(devOutm), &devOutm););
    checkErrorEx(errcode = clSetKernelArg(prg.kernel, 2, sizeof(k), &k););

    errcode = clEnqueueNDRangeKernel(dev.queue, prg.kernel, 2, NULL, glSz, locSz, 0, NULL, NULL);
    checkError(clEnqueueNDRangeKernel);
    clFinish(dev.queue);

    float* res = (float*)devOut.switchToHost();
    if (!compare(out, res, n))
        std::cout << "result mismatch!" << std::endl;

    releaseProgram(prg);
}

double testGrammian(dlmcl::Device dev, dlmcl::Memory* in, dlmcl::Memory* out, size_t vecSz, size_t n)
{
    const int itCount = 50;
	const int inSz = vecSz * n * sizeof(float);
	const int outSz = n * n * sizeof(float);
    cl_int errcode;

    Program prg = buildProgram(dev, DLM_SAMPLE_DIR "grammian.cl", "computeGramMrx");
    char* tmp = new char[out->getSize()];

    const size_t glSz[] = {(size_t)n, (size_t)n};
    const size_t locSz[] = {8ul, 8ul};
    clFinish(dev.queue);

    Timer t;
    for (int it = 0; it < itCount; ++it) {
        void* inMem = in->switchToHost();
        memset(inMem, 0xdeadbeef, in->getSize());

        cl_mem devIn = in->switchToDevice();
        checkErrorEx(errcode = clSetKernelArg(prg.kernel, 0, sizeof(devIn), &devIn););

        cl_mem devOut = out->switchToDevice();
        checkErrorEx(errcode = clSetKernelArg(prg.kernel, 1, sizeof(devOut), &devOut););
        checkErrorEx(errcode = clSetKernelArg(prg.kernel, 2, sizeof(vecSz), &vecSz););

        errcode = clEnqueueNDRangeKernel(dev.queue, prg.kernel, 2, NULL, glSz, locSz, 0, NULL, NULL);
        checkError(clEnqueueNDRangeKernel);
        void* res = out->switchToHost();
        //memcpy(tmp, res, out->getSize());
    }
    clFinish(dev.queue);
    t.stop();

    testc(dev, n, vecSz);

    releaseProgram(prg);
    delete [] tmp;
    return t.get();
}
