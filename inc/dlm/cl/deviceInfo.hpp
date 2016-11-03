#ifndef DLM_CL_DEVICE_INFO_HPP_
#define DLM_CL_DEVICE_INFO_HPP_
#include "dlm/cl/base.hpp"

namespace dlmcl {

enum DLM_DEVICE_VENDOR
{
    CDV_UNKNOWN = 0,
    CDV_AMD,
    CDV_INTEL,
    CDV_NVIDIA
};

enum DLM_DEVICE_TYPE
{
    DT_UNKNOWN = 0,
    DT_CPU,
    DT_GPU,
    DT_IGPU
};

enum DLM_MEMORY_TYPE
{
    MT_GENERIC = 0,
    MT_HOST,
    MT_DEVICE
};

class DeviceInfo
{
protected:
    cl_device_id device;

    void initDeviceVendor(void);
    void initDeviceType(void);

    void setDefault(void);
    void initialize(cl_device_id device);

    void operator=(const DeviceInfo&) = delete;
public:
    DeviceInfo(void) = delete;
    DeviceInfo(const DeviceInfo&) = default;
    explicit DeviceInfo(cl_device_id device) {
        initialize(device);
    };

    enum DLM_DEVICE_VENDOR  vendor;
    enum DLM_DEVICE_TYPE    type;
    cl_uint executionWidth; // 0 - can't determine width
    cl_uint globalMemoryBanks;
    cl_uint localMemoryBanks;

    bool supportMemoryType(enum DLM_MEMORY_TYPE memType) const;
};

#ifndef DLM_CL_SKIP_DEVICE_AMD

    struct DeviceInfoAMD {
        static void initDeviceInfo(const cl_device_id device, DeviceInfo& deviceInfo);
        static bool isSupportDeviceMemory(const cl_device_id device);
    };
#endif


} // ::dlmcl
#endif // DLM_CL_DEVICE_INFO_HPP_
