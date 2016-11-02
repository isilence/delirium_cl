#include <iostream>
#include "utils.hpp"
using namespace dlmcl;

const char* attributeNames[] = { "Name", "Vendor", "Version", "Profile"}; // , "Extensions" };
const cl_platform_info attributeTypes[5] = { CL_PLATFORM_NAME, CL_PLATFORM_VENDOR, CL_PLATFORM_VERSION, CL_PLATFORM_PROFILE, CL_PLATFORM_EXTENSIONS };
int attributeCount = sizeof(attributeNames)/sizeof(attributeNames[0]);

void printDeviceInfo(Device device) {
    size_t infoSize;
    char* info;
    for (int j = 0; j < attributeCount; j++) {
        // get platform attribute value size
        clGetPlatformInfo(device.platform, attributeTypes[j], 0, NULL, &infoSize);
        info = (char*) malloc(infoSize);

        // get platform attribute value
        clGetPlatformInfo(device.platform, attributeTypes[j], infoSize, info, NULL);

        printf(" .%d %-11s: %s\n", j+1, attributeNames[j], info);
        free(info);

    }

    // Extensions
    // get platform attribute value size
    //clGetDeviceInfo(device.device, CL_DEVICE_EXTENSIONS, 0, NULL, &infoSize);
    //info = (char*) malloc(infoSize);

    // get platform attribute value
    // clGetDeviceInfo(clDevice, CL_DEVICE_EXTENSIONS, infoSize, info, NULL);

    // printf(" .%d %-11s: %s\n", 0, "dev ext", info);
    //free(info);
}

// ===========================================================
//  init / release

void initDevice(Device& dev, int device_index) {
    int platform_index = 0;
    cl_int errcode;

    cl_platform_id platform[10];
    cl_uint num_platforms;
    errcode = clGetPlatformIDs(10, platform, &num_platforms);

    checkError(clGetPlatformIDs);
    printf("OpenCL platforms found: %d\n", num_platforms);

    dev.platform = platform[platform_index];
    cl_device_id devices[10];
    cl_uint num_devices;
    clGetDeviceIDs(platform[platform_index], CL_DEVICE_TYPE_GPU, 10, devices, &num_devices);
    printf("GPGPU devices found: %d\n", num_devices);
    if ((cl_uint)device_index > num_devices) {
        std::cout << "ocl compaitable gpu not found!" << std::endl;
        exit(1);
    }

    size_t valueSize;
    clGetDeviceInfo(devices[device_index], CL_DEVICE_NAME, 0, NULL, &valueSize);
    char* value = new char[valueSize];
    clGetDeviceInfo(devices[device_index], CL_DEVICE_NAME, valueSize, value, NULL);
    std::cout << "Use device #" << device_index << " : " << value << std::endl;
    delete[] value;

    dev.device = devices[device_index];
}

void initContext(dlmcl::Device& dev) {
    cl_int errcode;
    cl_context context;
    context = clCreateContext(NULL, 1, &dev.device, NULL, NULL, &errcode);
    dev.context = context;
    checkError(clCreateContext);
}

void initQueue(dlmcl::Device& dev) {
    cl_int errcode;
    cl_command_queue queue = clCreateCommandQueue(dev.context, dev.device, CL_QUEUE_PROFILING_ENABLE, &errcode);
    dev.queue = queue;
    checkError(clCreateCommandQueue);
}

void initOpenCl(dlmcl::Device& dev, int devIdx) {
    initDevice(dev, devIdx);
    initContext(dev);
    initQueue(dev);
}

void releaseOpenCl(dlmcl::Device dev) {
    clReleaseCommandQueue(dev.queue);
    clReleaseContext(dev.context);
    clReleaseDevice(dev.device);
}

