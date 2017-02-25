#include <string>
#include <string.h>

#include <iostream>
#include <fstream>
#include "../utils/utils.hpp"
#include "../utils/tasks.hpp"
#include "dlm/cl/deviceInfo.hpp"

using namespace dlmcl;
using namespace std;

enum MEMORY_TYPE types[] = {MT_GENERIC, MT_HOST, MT_DEVICE};
const char* typeNames[] = {"generic", "host", "device"};

string getVendorName(Device& dev)
{
    size_t infoSize;
    clGetPlatformInfo(dev.platform, CL_PLATFORM_VENDOR, 0, NULL, &infoSize);
    char* info = new char[infoSize];

    // get platform attribute value
    clGetPlatformInfo(dev.platform, CL_PLATFORM_VENDOR, infoSize, info, NULL);
    string res = info;
    delete [] info;
    return res;
}

string getDeviceName(Device& dev)
{
    size_t valueSize;
    clGetDeviceInfo(dev.device, CL_DEVICE_NAME, 0, NULL, &valueSize);
    char* value = new char[valueSize];
    clGetDeviceInfo(dev.device, CL_DEVICE_NAME, valueSize, value, NULL);

    string res = value;
    delete[] value;
    return res;
}

void generateCopyData(Device& dev, enum MEMORY_TYPE type, const char* typeName)
{
    if (type == MT_DEVICE)
        return;

    string fname = "Copy_" + getVendorName(dev) + "_" + getDeviceName(dev) + "_" + typeName;
    ofstream fd(fname);
    cout << fname << endl;

    for (size_t sz = 2; sz <= 32; sz+=2) {
        fd << sz << "," << Benchmark::run(new SCopy(dev, sz, type)) << "\n";
        cout << sz << std::endl;
    }
}


void generateGramData(Device& dev, enum MEMORY_TYPE type1, enum MEMORY_TYPE type2, const char* typeName1, const char* typeName2)
{
    if (type2 == MT_DEVICE)
        return;

    string fname = "Gramm_" + getVendorName(dev) + "_" + getDeviceName(dev) + "_" + typeName1 + "_" + typeName2;
    ofstream fd(fname);
    cout << fname << endl;

    size_t k = 1024;
    for (size_t n = 32; n <= 256; n+=16) {
        fd << n << "," << Benchmark::run(new Grammian(dev, n, k, type1, type2)) << "\n";
        cout << n << std::endl;
    }
}

void generateSqData(Device& dev, enum MEMORY_TYPE type1, enum MEMORY_TYPE type2, const char* typeName1, const char* typeName2)
{
    if (type2 == MT_DEVICE)
        return;

    string fname = "Sq_" + getVendorName(dev) + "_" + getDeviceName(dev) + "_" + typeName1 + "_" + typeName2;
    ofstream fd(fname);
    cout << fname << endl;

    for (size_t n = 1024*1024; n <= 1024*1024*16; n+=1024*1024) {
        fd << n/(1024*1024) << "," << Benchmark::run(new Square(dev, n, type1, type2)) << "\n";
        cout << n << std::endl;
    }
}


void testDevice(cl_device_id device)
{
    dlmcl::Device dev(device);

    for (size_t i=0; i<sizeof(types)/sizeof(types[0]); ++i)
        if (dev.info.mem.supportedTypes & types[i])
            generateCopyData(dev, types[i], typeNames[i]);
    generateCopyData(dev, (MEMORY_TYPE)-1, "auto");

    for (size_t i=0; i<sizeof(types)/sizeof(types[0]); ++i)
        for (size_t j=0; j<sizeof(types)/sizeof(types[0]); ++j)
            if ((dev.info.mem.supportedTypes & types[i]) && (dev.info.mem.supportedTypes & types[j]))
                generateGramData(dev, types[i], types[j], typeNames[i], typeNames[j]);
    generateGramData(dev, (MEMORY_TYPE)-1, (MEMORY_TYPE)-1, "auto", "auto");

    for (size_t i=0; i<sizeof(types)/sizeof(types[0]); ++i)
        for (size_t j=0; j<sizeof(types)/sizeof(types[0]); ++j)
            if ((dev.info.mem.supportedTypes & types[i]) && (dev.info.mem.supportedTypes & types[j]))
                generateSqData(dev, types[i], types[j], typeNames[i], typeNames[j]);
    generateSqData(dev, (MEMORY_TYPE)-1, (MEMORY_TYPE)-1, "auto", "auto");
}

int main(void)
{
    cl_int errcode;
    cl_platform_id platform[10];
    cl_uint platformsNum;
    errcode = clGetPlatformIDs(10, platform, &platformsNum);
    checkError(clGetPlatformIDs);

    for (cl_uint platformIdx = 0; platformIdx < platformsNum; ++platformIdx) {
        cl_device_id devices[10];
        cl_uint devicesNum;
        clGetDeviceIDs(platform[platformIdx], CL_DEVICE_TYPE_CPU, 10, devices, &devicesNum);

        for (cl_uint deviceIdx = 0; deviceIdx < devicesNum; ++deviceIdx)
            testDevice(devices[deviceIdx]);
    }

    return 0;
}
