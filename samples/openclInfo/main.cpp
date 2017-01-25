#include <string>
#include <string.h>

#include <iostream>
#include <fstream>
#include "../utils/utils.hpp"
#include "../utils/tasks.hpp"
#include "dlm/cl/deviceInfo.hpp"
#include "dlm/cl/kernel.hpp"

using namespace dlmcl;
using namespace std;

void testDevice(cl_device_id device)
{
    dlmcl::Device dev(device);
    printDeviceInfo(dev);

    cout    << "CU num: " << dev.info.comp.computeUnitNum << std::endl
            << "is sma: " << dev.info.mem.isSMA << std::endl;
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
