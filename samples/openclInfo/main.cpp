#include <string>
#include <iostream>
#include <fstream>

#include "dlm/cl/device.hpp"
#include "../utils/utils.hpp"
#include "../utils/tasks.hpp"

using namespace dlmcl;
using namespace std;

void printDeviceName(cl_device_id device, int idx)
{
    size_t nameLength;
    const cl_int errcode = clGetDeviceInfo(device, CL_DEVICE_NAME, 0, NULL, &nameLength);
    checkError(clGetDeviceInfo);

    char* value = new char[nameLength];
    clGetDeviceInfo(device, CL_DEVICE_NAME, nameLength, value, NULL);
    checkError(clGetDeviceInfo);

    std::cout << "Use device #" << idx << " : " << value << std::endl;
    delete[] value;
}

void testDevice(cl_device_id device)
{
    Device dev(device);
    printDeviceInfo(dev);

    cout    << "CU num: " << dev.info.compute.units << std::endl
            << "is sma: " << dev.info.memory.isSMA << std::endl;
}

void processPlatform(cl_platform_id platform)
{
    const int MAX_DEVICES = 10;
    cl_device_id devices[MAX_DEVICES];
    cl_uint devicesNum;

    const cl_int errcode = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 10, devices, &devicesNum);
    checkError(clGetDeviceIDs);
    printf("GPGPU devices found: %i\n", (int)devicesNum);

    for (cl_uint idx = 0; idx < devicesNum; ++idx)
    {
        printDeviceName(devices[idx], idx);
        testDevice(devices[idx]);
        std::cout << "\n=============================\n" << std::endl;
    }
}

int main(void)
{
    const int MAX_PLATFORMS = 10;
    cl_platform_id platforms[MAX_PLATFORMS];
    cl_uint platformsNum;
    const cl_int errcode = clGetPlatformIDs(MAX_PLATFORMS, platforms, &platformsNum);

    checkError(clGetPlatformIDs);
    printf("OpenCL platforms found: %i\n", (int)platformsNum);

    for (cl_uint idx = 0; idx < platformsNum; ++idx)
        processPlatform(platforms[idx]);

    return 0;
}
