#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <string>
#include <string.h>
#include <chrono>

#include <iostream>
#include "utils.hpp"
#include "dlm/cl/memory.hpp"
#include "dlm/cl/deviceInfo.hpp"

using namespace dlmcl;


void testDevice(cl_device_id device)
{
    dlmcl::Device dev(device);
    printDeviceInfo(dev);

    const size_t memsize = 1024*512;
    std::cout << "== copy test (ms) ==" << std::endl;
    {
        dlmcl::GenericMemory mem(dev, memsize, CL_MEM_READ_WRITE);
        std::cout << "gen: " << testCopy(dev, &mem) << std::endl;
    }
    {
        dlmcl::HostMemory mem(dev, memsize, CL_MEM_READ_WRITE);
        std::cout << "host: " << testCopy(dev, &mem) << std::endl;
    }


    // grammian
    size_t n = 2*128;
    const size_t k = 1024;
    size_t isz = n*k*sizeof(float);
    size_t osz = n*n*sizeof(float);

    std::cout << "== grammian calc test (ms) ==" << std::endl;
    {
        GenericMemory memIn(dev, isz, CL_MEM_READ_ONLY);
        GenericMemory memOut(dev, osz, CL_MEM_WRITE_ONLY);
        std::cout << "gen to gen: " << testGrammian(dev, &memIn, &memOut, k, n) << std::endl;
    }
    {
        HostMemory memIn(dev, isz, CL_MEM_READ_ONLY);
        HostMemory memOut(dev, osz, CL_MEM_WRITE_ONLY);
        std::cout << "host to host: " << testGrammian(dev, &memIn, &memOut, k, n) << std::endl;
    }
    {
        HostMemory memIn(dev, isz, CL_MEM_READ_ONLY);
        GenericMemory memOut(dev, osz, CL_MEM_WRITE_ONLY);
        std::cout << "host to gen: " << testGrammian(dev, &memIn, &memOut, k, n) << std::endl;
    }
    {
        GenericMemory memIn(dev, isz, CL_MEM_READ_ONLY);
        HostMemory memOut(dev, osz, CL_MEM_WRITE_ONLY);
        std::cout << "gen to host: " << testGrammian(dev, &memIn, &memOut, k, n) << std::endl;
    }
    if (dev.info.supportMemoryType(MT_DEVICE)) {
        {
            DeviceMemory memIn(dev, isz, CL_MEM_READ_ONLY);
            GenericMemory memOut(dev, osz, CL_MEM_WRITE_ONLY);
            std::cout << "dev to gen: " << testGrammian(dev, &memIn, &memOut, k, n) << std::endl;
        }
        {
            DeviceMemory memIn(dev, isz, CL_MEM_READ_ONLY);
            HostMemory memOut(dev, osz, CL_MEM_WRITE_ONLY);
            std::cout << "dev to host: " << testGrammian(dev, &memIn, &memOut, k, n) << std::endl;
        }
    }

    {
        Memory* memIn = getOptimalMemory(dev, isz, CL_MEM_READ_ONLY);
        Memory* memOut = getOptimalMemory(dev, osz, CL_MEM_WRITE_ONLY);
        std::cout << "auto to auto: " << testGrammian(dev, memIn, memOut, k, n) << std::endl;
        delete memIn;
        delete memOut;
    }
    // squary array
    n = 1024*1024;
    size_t sz = n*sizeof(float);

    std::cout << "== sq test (ms) ==" << std::endl;
    {
            GenericMemory memIn(dev, sz, CL_MEM_READ_ONLY);
            GenericMemory memOut(dev, sz, CL_MEM_WRITE_ONLY);
            std::cout << "gen to gen: " << testSq(dev, &memIn, &memOut, n) << std::endl;
    }
    {
            HostMemory memIn(dev, sz, CL_MEM_READ_ONLY);
            HostMemory memOut(dev, sz, CL_MEM_WRITE_ONLY);
            std::cout << "host to host: " << testSq(dev, &memIn, &memOut, n) << std::endl;
    }
    {
            HostMemory memIn(dev, sz, CL_MEM_READ_ONLY);
            GenericMemory memOut(dev, sz, CL_MEM_WRITE_ONLY);
            std::cout << "host to gen: " << testSq(dev, &memIn, &memOut, n) << std::endl;
    }
    {
            GenericMemory memIn(dev, sz, CL_MEM_READ_ONLY);
            HostMemory memOut(dev, sz, CL_MEM_WRITE_ONLY);
            std::cout << "gen to host: " << testSq(dev, &memIn, &memOut, n) << std::endl;
    }
    if (dev.info.supportMemoryType(MT_DEVICE)) {
        {
                DeviceMemory memIn(dev, sz, CL_MEM_READ_ONLY);
                GenericMemory memOut(dev, sz, CL_MEM_WRITE_ONLY);
                std::cout << "dev to gen: " << testSq(dev, &memIn, &memOut, n) << std::endl;
        }
        {
                DeviceMemory memIn(dev, sz, CL_MEM_READ_ONLY);
                HostMemory memOut(dev, sz, CL_MEM_WRITE_ONLY);
                std::cout << "dev to host: " << testSq(dev, &memIn, &memOut, n) << std::endl;
        }
    }
    {
        Memory* memIn = getOptimalMemory(dev, sz, CL_MEM_READ_ONLY);
        Memory* memOut = getOptimalMemory(dev, sz, CL_MEM_WRITE_ONLY);
        std::cout << "auto to auto: " << testSq(dev, memIn, memOut, n) << std::endl;
        delete memIn;
        delete memOut;
    }
}

int main(void)
{
    cl_int errcode;
    cl_platform_id platform[10];
    cl_uint platformsNum;
    errcode = clGetPlatformIDs(10, platform, &platformsNum);

    checkError(clGetPlatformIDs);
    printf("OpenCL platforms found: %i\n", (int)platformsNum);

    for (cl_uint platformIdx = 0; platformIdx < platformsNum; ++platformIdx) {
        cl_device_id devices[10];
        cl_uint devicesNum;
        clGetDeviceIDs(platform[platformIdx], CL_DEVICE_TYPE_ALL, 10, devices, &devicesNum);
        printf("GPGPU devices found: %i\n", (int)devicesNum);

        for (cl_uint deviceIdx = 0; deviceIdx < devicesNum; ++deviceIdx)
        {
            size_t valueSize;
            clGetDeviceInfo(devices[deviceIdx], CL_DEVICE_NAME, 0, NULL, &valueSize);
            char* value = new char[valueSize];
            clGetDeviceInfo(devices[deviceIdx], CL_DEVICE_NAME, valueSize, value, NULL);
            std::cout << "Use device #" << deviceIdx << " : " << value << std::endl;
            delete[] value;

            testDevice(devices[deviceIdx]);
            std::cout << "\n=============================\n" << std::endl;
        }
    }

    return 0;
}
