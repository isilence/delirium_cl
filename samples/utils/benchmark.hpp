#ifndef SAMPLES_UTILS_BENCHMARK_HPP_
#define SAMPLES_UTILS_BENCHMARK_HPP_
#include <math.h>
#include <string.h>
#include <memory>

#include "utils.hpp"
#include "dlm/cl/memobj.hpp"

class Task
{
public:
    virtual ~Task(void) {}
    virtual void initialize() = 0;
    virtual void release() = 0;
    virtual void run() = 0;

    static dlmcl::Memobj* constructMemory(dlmcl::Device& dev, size_t size, cl_mem_flags access, enum dlmcl::MEMORY_TYPE type)
    {
        using namespace dlmcl;
        switch (type)
        {
        case dlmcl::MT_GENERIC:
            return new GenericMemory(dev, size, access);
        case dlmcl::MT_HOST:
            return new HostMemory(dev, size, access);
        case dlmcl::MT_DEVICE:
            return new DeviceMemory(dev, size, access);
        default:
            return Memobj::getOptimal(dev, size, access);
        }
    }
};

class Benchmark {
public:
    static double run(Task* task, size_t maxItNum = 500, double maxTime = 4000.0)
    {
        double dt = 0.0;
        size_t itCnt = 0;
        Timer t;

        task->initialize();
        while (itCnt < maxItNum && dt < maxTime)
        {
            t.start();
            for (int i=0; i<30; ++i)
                task->run();
            t.stop();
            dt += t.get();
            itCnt += 30;
        }
        task->release();

        delete task;
        return dt / itCnt;
    }
};


#endif // SAMPLES_UTILS_BENCHMARK_HPP_
