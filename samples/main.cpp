#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <string>
#include <string.h>

#include <iostream>
#include "utils.hpp"
#include "dlm/cl/memory.hpp"

using namespace std;
using namespace dlmcl;


float testCopy(dlmcl::Device dev, size_t memsize, dlmcl::Memory* mem)
{
    const int itCount = 1000;
    void* tmp = new char[mem->getSize()];

    clock_t t0 = clock();
    for (int i=0; i<itCount; ++i) {
        void* buffer = mem->switchToHost();
        memcpy(tmp, buffer, mem->getSize());
        memset(buffer, 0xdeadbeef, mem->getSize());
        mem->switchToDevice();
    }
    clFinish(dev.queue);
    clock_t t1 = clock();

    delete [] tmp;
    float elapsedTimeGPU = (t1-t0)/(float)CLOCKS_PER_SEC;
    return elapsedTimeGPU * 1000;
}

float testGrammian(dlmcl::Device dev, dlmcl::Memory* in, dlmcl::Memory* out, size_t vecSz, size_t n)
{
    const int itCount = 100;
	const int inSz = vecSz * n * sizeof(float);
	const int outSz = n * n * sizeof(float);
    cl_int errcode;

    Program prg = buildProgram(dev, DLM_SAMPLE_DIR "grammian.cl", "computeGramMrx");
    char* tmp = new char[out->getSize()];

    const size_t glSz[] = {(size_t)n, (size_t)n};
    const size_t locSz[] = {8ul, 8ul};

    clock_t t0 = clock();
    for (int it = 0; it < itCount; ++it) {
        void* inMem = in->switchToHost();
        // memset(inMem, 0xdeadbeef, in->getSize());

        cl_mem devIn = in->switchToDevice();
        checkErrorEx(errcode = clSetKernelArg(prg.kernel, 0, sizeof(devIn), &devIn););

        cl_mem devOut = out->switchToDevice();
        checkErrorEx(errcode = clSetKernelArg(prg.kernel, 1, sizeof(devOut), &devOut););
        checkErrorEx(errcode = clSetKernelArg(prg.kernel, 2, sizeof(vecSz), &vecSz););

        errcode = clEnqueueNDRangeKernel(dev.queue, prg.kernel, 2, NULL, glSz, locSz, 0, NULL, NULL);
        checkError(clEnqueueNDRangeKernel);
        void* res = out->switchToHost();
        // memcpy(tmp, res, out->getSize());
    }
    clFinish(dev.queue);
    clock_t t1 = clock();

    releaseProgram(prg);
    delete [] tmp;
    float elapsedTimeGPU = (t1-t0)/(float)CLOCKS_PER_SEC;
    return elapsedTimeGPU * 1000;
}

void testDevice(int num)
{

    dlmcl::Device dev;
    initOpenCl(dev, num);
    printDeviceInfo(dev);

    // copy
    const size_t memsize = 1024*512;
    const float dt1 = testCopy(dev, memsize, &dlmcl::GenericMemory(dev, memsize));
    std::cout << "copy generic: " << dt1 << std::endl;

    const float dt2 = testCopy(dev, memsize, &dlmcl::HostMemobj(dev, memsize));
    std::cout << "copy hostmem: " << dt2 << std::endl;

    // grammian
    const size_t n = 2*128;
    const size_t k = 1024;
    size_t isz = n*k*sizeof(float);
    size_t osz = n*n*sizeof(float);
    const float dt3 = testGrammian(dev, &GenericMemory(dev, isz), &GenericMemory(dev, osz), k, n);
    std::cout << "grammian generic: " << dt3 << std::endl;

    const float dt4 = testGrammian(dev, &HostMemobj(dev, isz), &HostMemobj(dev, osz), k, n);
    std::cout << "grammian hostmem: " << dt4 << std::endl;

    const float dt5 = testGrammian(dev, &HostMemobj(dev, isz), &GenericMemory(dev, osz), k, n);
    std::cout << "grammian mixed: " << dt5 << std::endl;

    const float dt6 = testGrammian(dev, &GenericMemory(dev, isz), &HostMemobj(dev, osz), k, n);
    std::cout << "grammian mixed: " << dt6 << std::endl;
}

int main(void)
{
    testDevice(0);
    std::cout << "\n=============================\n" << std::endl;
    testDevice(1);

    return 0;
}
