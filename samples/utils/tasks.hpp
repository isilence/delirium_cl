#ifndef SAMPLES_TESTS_HPP_
#define SAMPLES_TESTS_HPP_
#include "../utils/benchmark.hpp"

class Square : public Task
{
    dlmcl::Device& dev;
    size_t n;
    char* tmp;

    dlmcl::DLM_MEMORY_TYPE intype, outtype;

    cl_command_queue queue;
    Program prg;
    dlmcl::Memory *memIn, *memOut;


public:
    Square(dlmcl::Device& dev, size_t n, dlmcl::DLM_MEMORY_TYPE intype, dlmcl::DLM_MEMORY_TYPE outtype) :
        dev(dev), n(n), intype(intype), outtype(outtype)
    {}

    void setSize(size_t _n) {
         n = _n;
    }

    virtual void initialize() override
    {
        prg = buildProgram(dev, DLM_SAMPLE_DIR "/kernels/simple.cl", "squareArray");
        queue = clCreateCommandQueueWithProperties(dev.context, dev.device, nullptr, NULL);

        memIn = Task::constructMemory(dev, n*sizeof(float), CL_MEM_READ_ONLY, intype);
        memOut = Task::constructMemory(dev, n*sizeof(float), CL_MEM_WRITE_ONLY, outtype);
        tmp = new char[memOut->getSize()];
    }
    virtual void release() override
    {
        delete memIn;
        delete memOut;
        delete [] tmp;
        clReleaseCommandQueue(queue);
        releaseProgram(prg);
    }

    virtual void run() override
    {
        static const size_t loc[] = {64ul};

        // fill mem
        memIn->switchToHost(queue);
        memset(memIn->getHostMemory(), (unsigned int)0xdeadbeef, memIn->getSize()); //-V575

        // run kernel
        memIn->switchToDevice(queue);
        memOut->switchToDevice(queue);
        cl_mem devOut = memOut->getDeviceMemory();
        cl_mem devIn = memIn->getDeviceMemory();
        clSetKernelArg(prg.kernel, 1, sizeof(devOut), &devOut);
        clSetKernelArg(prg.kernel, 0, sizeof(devIn), &devIn);
        clEnqueueNDRangeKernel(queue, prg.kernel, 1, NULL, &n, loc, 0, NULL, NULL);

        // gather results
        memOut->switchToHost(queue);
        memcpy(tmp, memOut->getHostMemory(), memOut->getSize());
    }
};

class Grammian : public Task
{
    dlmcl::Device& dev;
    size_t n, k;
    char* tmp;
    dlmcl::DLM_MEMORY_TYPE intype, outtype;

    Program prg;
    cl_command_queue queue;
    dlmcl::Memory *memIn, *memOut;

public:
    Grammian(dlmcl::Device& dev, size_t n, size_t k, dlmcl::DLM_MEMORY_TYPE intype, dlmcl::DLM_MEMORY_TYPE outtype) :
        dev(dev), n(n), k(k), intype(intype), outtype(outtype)
    {}

    void setSize(size_t _n, size_t _k) {
         n = _n;
         k = _k;
    }

    virtual void initialize() override
    {
        prg = buildProgram(dev, DLM_SAMPLE_DIR "/kernels/grammian.cl", "computeGramMrx");
        queue = clCreateCommandQueueWithProperties(dev.context, dev.device, nullptr, NULL);

        memIn = Task::constructMemory(dev, n*k*sizeof(float), CL_MEM_READ_ONLY, intype);
        memOut = Task::constructMemory(dev, n*n*sizeof(float), CL_MEM_WRITE_ONLY, outtype);
        tmp = new char[memOut->getSize()];
    }
    virtual void release() override
    {
        delete memIn;
        delete memOut;
        delete [] tmp;
        clReleaseCommandQueue(queue);
        releaseProgram(prg);
    }

    virtual void run() override
    {
        // fill buffers
        memIn->switchToHost(queue);
        memset(memIn->getHostMemory(), (unsigned int)0xdeadbeef, memIn->getSize()); //-V575

        // prepare kernel data
        memIn->switchContext(queue, dlmcl::Memory::MCT_DEVICE);
        memOut->switchContext(queue, dlmcl::Memory::MCT_DEVICE);
        cl_mem devIn = memIn->getDeviceMemory();
        cl_mem devOut = memOut->getDeviceMemory();

        clSetKernelArg(prg.kernel, 0, sizeof(devIn), &devIn);
        clSetKernelArg(prg.kernel, 1, sizeof(devOut), &devOut);
        clSetKernelArg(prg.kernel, 2, sizeof(k), &k);

        // run kernel
        const size_t glob[] = {n, n}, loc[] = {8ul, 8ul};
        clEnqueueNDRangeKernel(queue, prg.kernel, 2, NULL, glob, loc, 0, NULL, NULL);

        // copy data back
        memOut->switchToHost(queue);
        memcpy(tmp, memOut->getHostMemory(), memOut->getSize());
    }
};

class SCopy : public Task
{
    dlmcl::Device& dev;
    char* tmp;
    size_t n;

    cl_command_queue queue;
    dlmcl::Memory *mem;
    dlmcl::DLM_MEMORY_TYPE type;

public:
    SCopy(dlmcl::Device& dev, size_t n, dlmcl::DLM_MEMORY_TYPE type) :
        dev(dev), n(n), type(type)
    {}
    void setSize(size_t _n) {
         n = _n;
    }

    virtual void initialize() override
    {
        queue = clCreateCommandQueueWithProperties(dev.context, dev.device, nullptr, NULL);
        mem = Task::constructMemory(dev, 1024*1024*n, CL_MEM_READ_WRITE, type);
        tmp = new char[mem->getSize()];
    }
    virtual void release() override
    {
        delete mem;
        delete [] tmp;
        clReleaseCommandQueue(queue);
    }

    virtual void run() override
    {
        mem->switchToHost(queue);
        //memcpy(tmp, mem->getHostMemory(), mem->getSize());
        memset(mem->getHostMemory(), (unsigned int)0xdeadbeef, mem->getSize()); //-V575
        mem->switchToDevice(queue);
    }
};


#endif // SAMPLES_TESTS_HPP_
