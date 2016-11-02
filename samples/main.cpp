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

using namespace dlmcl;

void testDevice(int num)
{

    dlmcl::Device dev;
    initOpenCl(dev, num);
    printDeviceInfo(dev);

        // copy
    const size_t memsize = 1024*512;
    std::cout << "== copy test (ms) ==" << std::endl;

    std::cout << "gen: "
        << testCopy(dev, memsize, &dlmcl::GenericMemory(dev, memsize, CL_MEM_READ_WRITE))
        << std::endl;

    std::cout << "host: "
        << testCopy(dev, memsize, &dlmcl::HostMemory(dev, memsize, CL_MEM_READ_WRITE))
        << std::endl;

    std::cout << "device: "
        << testCopy(dev, memsize, &dlmcl::DeviceMemory(dev, memsize, CL_MEM_READ_WRITE))
        << std::endl;

    // grammian
    size_t n = 2*128;
    const size_t k = 1024;
    size_t isz = n*k*sizeof(float);
    size_t osz = n*n*sizeof(float);

    std::cout << "== grammian calc test (ms) ==" << std::endl;
    std::cout << "gen to gen: "
        << testGrammian(dev, &GenericMemory(dev, isz, CL_MEM_READ_ONLY), &GenericMemory(dev, osz, CL_MEM_WRITE_ONLY), k, n)
        << std::endl;

    std::cout << "host to host: "
        << testGrammian(dev, &HostMemory(dev, isz, CL_MEM_READ_ONLY), &HostMemory(dev, osz, CL_MEM_WRITE_ONLY), k, n)
        << std::endl;

    std::cout << "host to gen: "
        << testGrammian(dev, &HostMemory(dev, isz, CL_MEM_READ_ONLY), &GenericMemory(dev, osz, CL_MEM_WRITE_ONLY), k, n)
        << std::endl;

    std::cout << "gen to host: "
        << testGrammian(dev, &GenericMemory(dev, isz, CL_MEM_READ_ONLY), &HostMemory(dev, osz, CL_MEM_WRITE_ONLY), k, n)
        << std::endl;

    std::cout << "dev to gen: "
        << testGrammian(dev, &DeviceMemory(dev, isz, CL_MEM_READ_ONLY), &GenericMemory(dev, osz, CL_MEM_WRITE_ONLY), k, n)
        << std::endl;

    std::cout << "dev to host: "
        << testGrammian(dev, &DeviceMemory(dev, isz, CL_MEM_READ_ONLY), &HostMemory(dev, osz, CL_MEM_WRITE_ONLY), k, n)
        << std::endl;

    // squary array
    n = 1024*1024;
    size_t sz = n*sizeof(float);

    std::cout << "== sq test (ms) ==" << std::endl;
    std::cout << "gen to gen: "
        << testSq(dev, &GenericMemory(dev, sz, CL_MEM_READ_ONLY), &GenericMemory(dev, sz, CL_MEM_WRITE_ONLY), n)
        << std::endl;

    std::cout << "host to host: "
        << testSq(dev, &HostMemory(dev, sz, CL_MEM_READ_ONLY), &HostMemory(dev, sz, CL_MEM_WRITE_ONLY), n)
        << std::endl;

    std::cout << "host to gen: "
        << testSq(dev, &HostMemory(dev, sz, CL_MEM_READ_ONLY), &GenericMemory(dev, sz, CL_MEM_WRITE_ONLY), n)
        << std::endl;

    std::cout << "gen to host: "
        << testSq(dev, &GenericMemory(dev, sz, CL_MEM_READ_ONLY), &HostMemory(dev, sz, CL_MEM_WRITE_ONLY), n)
        << std::endl;

     std::cout << "dev to gen: "
        << testSq(dev, &DeviceMemory(dev, sz, CL_MEM_READ_ONLY), &GenericMemory(dev, sz, CL_MEM_WRITE_ONLY), n)
        << std::endl;

    std::cout << "dev to host: "
        << testSq(dev, &DeviceMemory(dev, sz, CL_MEM_READ_ONLY), &HostMemory(dev, sz, CL_MEM_WRITE_ONLY), n)
        << std::endl;
}

int main(void)
{
    testDevice(0);
    std::cout << "\n=============================\n" << std::endl;
    testDevice(1);

    return 0;
}
