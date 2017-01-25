#ifndef DLM_CL_DEVICE_INFO_HPP_
#define DLM_CL_DEVICE_INFO_HPP_
#include "dlm/cl/base.hpp"

namespace dlmcl {

enum DEVICE_VENDOR
{
    CDV_UNKNOWN = 0,
    CDV_AMD,
    CDV_INTEL,
    CDV_NVIDIA
};

enum MEMORY_TYPE
{
    MT_GENERIC  = 0x1,
    MT_HOST     = 0x2,
    MT_DEVICE   = 0x4
};

struct BusTopology
{
    int bus, dev, fn;
};

struct MemoryCap
{
    cl_ulong size;
    int bankNum;
};

struct MemoryInfo
{
    MemoryCap global;
    MemoryCap local;

    cl_ulong cacheSize;
    cl_ulong cacheLaneWidth;

    int supportedTypes;
    bool isSMA;
};

struct ComputeInfo
{
    int computeUnitNum;
    int waveFront;
    int maxGroup;
    int maxDim;
    int simdWidth;
};

struct DeviceInfo
{
    MemoryInfo mem;
    BusTopology top;
    ComputeInfo comp;

    cl_device_type type;
    enum DEVICE_VENDOR vendor;
};

class DeviceInfoFiller
{
protected:
    DeviceInfo& di;
    const cl_device_id dev;

    #if !defined(DLM_CL_SKIP_DEVICE_AMD)
        void fillAMD(void);
    #endif
    #if !defined(DLM_CL_SKIP_DEVICE_INTEL)
        void fillIntel(void);
    #endif

public:
    DeviceInfoFiller(DeviceInfo& di, cl_device_id dev)
        :   di(di),
            dev(dev) {}

    void fill(void);

    DeviceInfo& get(void) noexcept {
        return di;
    }
};


} // ::dlmcl
#endif // DLM_CL_DEVICE_INFO_HPP_
