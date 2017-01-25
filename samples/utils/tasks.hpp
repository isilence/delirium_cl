#ifndef SAMPLES_TESTS_HPP_
#define SAMPLES_TESTS_HPP_
#include "../utils/benchmark.hpp"
#include "dlm/cl/kernel.hpp"

class Square : public Task
{
    dlmcl::Device& dev;
    size_t n;
    char* tmp;

    dlmcl::MEMORY_TYPE intype, outtype;

    cl_command_queue queue;
    Program prg;
    dlmcl::Memobj *memIn, *memOut;


public:
    Square(dlmcl::Device& dev, size_t n, dlmcl::MEMORY_TYPE intype, dlmcl::MEMORY_TYPE outtype) :
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
        memIn->switchToHost(queue);
        memset(memIn->getHostMemory(), (unsigned int)0xdeadbeef, memIn->getSize()); //-V575

        const size_t loc = 64ul;
        dlmcl::runKernel(prg.kernel, queue, 1, &n, &loc, memIn, memOut);

        memOut->switchToHost(queue);
        memcpy(tmp, memOut->getHostMemory(), memOut->getSize());
    }
};

class Grammian : public Task
{
    dlmcl::Device& dev;
    size_t n, k;
    char* tmp;
    dlmcl::MEMORY_TYPE intype, outtype;

    Program prg;
    cl_command_queue queue;
    dlmcl::Memobj *memIn, *memOut;

public:
    Grammian(dlmcl::Device& dev, size_t n, size_t k, dlmcl::MEMORY_TYPE intype, dlmcl::MEMORY_TYPE outtype) :
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
        memIn->switchToHost(queue);
        memset(memIn->getHostMemory(), (unsigned int)0xdeadbeef, memIn->getSize()); //-V575

        const size_t glob[] = {n, n}, loc[] = {8ul, 8ul};
        dlmcl::runKernel(prg.kernel, queue, 2, glob, loc, memIn, memOut, k);

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
    dlmcl::Memobj *mem;
    dlmcl::MEMORY_TYPE type;

public:
    SCopy(dlmcl::Device& dev, size_t n, dlmcl::MEMORY_TYPE type) :
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
        memcpy(tmp, mem->getHostMemory(), mem->getSize());
        memset(mem->getHostMemory(), (unsigned int)0xdeadbeef, mem->getSize()); //-V575
        mem->switchToDevice(queue);
    }
};


#endif // SAMPLES_TESTS_HPP_
