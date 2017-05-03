#include <iostream>
#include <fstream>
#include <string.h>

#include "dlm/env/macro.h"
#include "utils.hpp"
using namespace dlmcl;

size_t fileSize(std::string path){
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        return 0;
    std::streampos fsize = file.tellg();
    file.close();
    return (size_t)fsize;
}

std::string loadFile(const std::string path) {
    const size_t size = fileSize(path);
    std::ifstream fd(path);
    if (!fd.is_open())
        throw -1;

    char* buffer = new char[size+1];
    memset(buffer, '\0', size+1);
    fd.read(buffer, size);

    std::string r(buffer);
    delete[] buffer;
    return r;
}


std::string getOptOptions(dlmcl::Device& dev)
{
    cl_uint vec_width;
    clGetDeviceInfo(dev.device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(vec_width), &vec_width, NULL);

    #define BLOCK_SIZE 8
    #define BLOCK_SIDEX 8
    #define BLOCK_SIDEY 8

    std::string opt = "-D " DLM_QUOTE_(BLOCK_SIDEX) "=" DLM_QUOTE(BLOCK_SIDEX);
    opt += " -D " DLM_QUOTE_(BLOCK_SIDEY) "=" DLM_QUOTE(BLOCK_SIDEY);
    opt += " -D DLM_ARCH_VECSZ_FLOAT=";
    opt += std::to_string(vec_width);

    return opt;
}

Program buildProgram(dlmcl::Device& dev, std::string path, std::string kernelName)
{
    std::string src = loadFile(path);
    cl_int errcode;
    const char* s = src.data();
    cl_program program = clCreateProgramWithSource(dev.context, 1, (const char **)&s, NULL, &errcode);
    checkError(clCreateProgramWithSource);

    std::string buildOptions = "-cl-fast-relaxed-math -cl-no-signed-zeros -cl-mad-enable ";
    buildOptions += " -O3 ";
    buildOptions += getOptOptions(dev);

    errcode = clBuildProgram(program, 1, &dev.device, buildOptions.data(), NULL, NULL);
    if (errcode != CL_SUCCESS) {
        size_t len;
        char *build_log;
        std::cout << "There were error during build kernel code. Please, check program code. Errcode = "
                  << errcode << std::endl;
        clGetProgramBuildInfo(program, dev.device, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
        build_log = new char[len+1];
        clGetProgramBuildInfo(program, dev.device, CL_PROGRAM_BUILD_LOG, len, build_log, NULL);
        build_log[len] = '\0';
        std::cout << "BUILD LOG: " << build_log << std::endl;
        delete[] build_log;
        exit(1);
    }
    cl_kernel kernel = clCreateKernel(program, kernelName.data(), &errcode);
    checkError(clCreateKernel);

    return Program{kernel, program};
}

void releaseProgram(Program p) {
    clReleaseKernel(p.kernel);
    clReleaseProgram(p.program);
}
