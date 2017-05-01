#include <string>
#include <string.h>

#include <iostream>
#include <fstream>
#include "../utils/utils.hpp"
#include "../utils/tasks.hpp"
#include "dlm/cl/device.hpp"
#include "dlm/cl/kernel.hpp"

using namespace dlmcl;
using namespace std;

void testDevice(cl_device_id device)
{
    dlmcl::Device dev(device);
    printDeviceInfo(dev);

    const size_t memsize = 128;
    std::cout << "== copy test (ms) ==" << std::endl;
    std::cout << "gen: " << Benchmark::run(new SCopy(dev, memsize, MT_GENERIC)) << std::endl;
    std::cout << "host: " << Benchmark::run(new SCopy(dev, memsize, MT_HOST)) << std::endl;

    //// grammian
    size_t n = 2*128;
    const size_t k = 2*1024;

    std::cout << "== grammian calc test (ms) ==" << std::endl;
    std::cout << "gen to gen: " << Benchmark::run(new Grammian(dev, n, k, MT_GENERIC, MT_GENERIC)) << std::endl;
    std::cout << "host to host: " << Benchmark::run(new Grammian(dev, n, k, MT_HOST, MT_HOST)) << std::endl;
    std::cout << "host to gen: " << Benchmark::run(new Grammian(dev, n, k, MT_HOST, MT_GENERIC)) << std::endl;
    std::cout << "gen to host: " << Benchmark::run(new Grammian(dev, n, k, MT_GENERIC, MT_HOST)) << std::endl;
    if (dev.info.memory.supportedTypes & MT_DEVICE) {
        std::cout << "dev to gen: " << Benchmark::run(new Grammian(dev, n, k, MT_DEVICE, MT_GENERIC)) << std::endl;
        std::cout << "dev to host: " << Benchmark::run(new Grammian(dev, n, k, MT_DEVICE, MT_HOST)) << std::endl;
    }
    std::cout << "auto to auto: " << Benchmark::run(new Grammian(dev, n, k, (MEMORY_TYPE)-1, (MEMORY_TYPE)-1)) << std::endl;


    // squary array
    n = 1024*1024;
    std::cout << "== sq test (ms) ==" << std::endl;
    std::cout << "gen to gen: " << Benchmark::run(new Square(dev, n, MT_GENERIC, MT_GENERIC)) << std::endl;
    std::cout << "host to host: " << Benchmark::run(new Square(dev, n, MT_HOST, MT_HOST)) << std::endl;
    std::cout << "host to gen: " << Benchmark::run(new Square(dev, n, MT_HOST, MT_GENERIC)) << std::endl;
    std::cout << "gen to host: " << Benchmark::run(new Square(dev, n, MT_GENERIC, MT_HOST)) << std::endl;
    if (dev.info.memory.supportedTypes & MT_DEVICE) {
        std::cout << "dev to gen: " << Benchmark::run(new Square(dev, n, MT_DEVICE, MT_GENERIC)) << std::endl;
        std::cout << "dev to host: " << Benchmark::run(new Square(dev, n, MT_DEVICE, MT_HOST)) << std::endl;
    }
    std::cout << "auto to auto: " << Benchmark::run(new Square(dev, n, (MEMORY_TYPE)-1, (MEMORY_TYPE)-1)) << std::endl;
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
