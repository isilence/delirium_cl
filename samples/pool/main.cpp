#include "../utils/utils.hpp"
#include "../utils/tasks.hpp"
#include "dlm/cl/controllers.hpp"
#include "dlm/cl/memobjPool.hpp"
#include "dlm/cl/kernel.hpp"

using namespace dlmcl;
using namespace std;

void testDevice(cl_device_id device)
{
    size_t n = 500;
    dlmcl::Device dev(device);
    dlmcl::MemobjPool pool;

    Program prg = buildProgram(dev, DLM_SAMPLE_DIR "/kernels/simple.cl", "squareArray");
    cl_command_queue queue = clCreateCommandQueue(dev.context, dev.device, 0, nullptr);

    float* in = pool.alloc<float>(dev, n, CL_MEM_READ_ONLY);
    float* dst = pool.alloc<float>(dev, n, CL_MEM_WRITE_ONLY);

    const size_t loc = 64ul;
    dlmcl::runKernel(prg.kernel, queue, 1, &n, &loc, pool, in, dst);
    pool.sync(dst, queue);

    // process result output data in dst ...
    pool.free(in);
    pool.free(dst);
}

int main(void)
{
    registerDefaultVendorControllers();

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
