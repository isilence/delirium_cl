#include <map>
#include <string>

#include "dlm/cl/device.hpp"
using namespace dlmcl;

const char Device::DEFAULT_VENDOR[Device::MAX_VENDOR_NAME] = "default";

// ======================================
//    device controller storage
// ======================================

namespace {
    class VendorStorage
    {
    public:
        typedef std::map<std::string, Controller*> TVendors;
        inline static TVendors& instance(void) noexcept
        {
            static TVendors vendors;
            return vendors;
        }

    private:
        ~VendorStorage()                                = delete;
        VendorStorage(void)                             = delete;
        VendorStorage(VendorStorage const&)             = delete;
        VendorStorage& operator=(VendorStorage const&)  = delete;
    };
}


int Device::registerVendorController(const char *vendorName, Controller *vendor) noexcept
{
    Controller*& oldVendor = VendorStorage::instance()[vendorName];
    const int ret = (oldVendor == nullptr) ? 1 : 0;

    oldVendor = vendor;
    return ret;
}


Controller* Device::getVendorController(const char *vendorName) noexcept
{
    typedef VendorStorage::TVendors TVendors;
    TVendors& vendors = VendorStorage::instance();
    TVendors::iterator it = VendorStorage::instance().find(vendorName);

    return (it != vendors.end()) ? it->second : nullptr;
}


// ======================================
//          helpers
// ======================================

static cl_platform_id getPlatform(cl_device_id clDevice)
{
    cl_platform_id clPlatform;
    const cl_int res = clGetDeviceInfo(clDevice, CL_DEVICE_PLATFORM, sizeof(clPlatform), &clPlatform, nullptr);

    if (res != CL_SUCCESS)
        throw new CLException();
    return clPlatform;
}

static Controller* findVendorController(cl_device_id clDevice)
{
    char vendorName[Device::MAX_VENDOR_NAME];
    Device::getVendorName(clDevice, vendorName, sizeof(vendorName));

    Controller* controller = Device::getVendorController(vendorName);
    if (controller == nullptr) {
        controller = Device::getVendorController(Device::DEFAULT_VENDOR);
        if (controller == nullptr) {
            throw CLException();
        }
    }
    return controller;
}


// ======================================
//          device impl
// ======================================

void Device::getVendorName(cl_device_id device, char* const buffer, const size_t size)
{
    const cl_int error = clGetDeviceInfo(device, CL_DEVICE_VENDOR, size, buffer, nullptr);
    if (error != CL_SUCCESS)
        throw CLException();
}

void Device::setDefaultContext(void)
{
    cl_int res;
    cl_context res_context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &res);

    if (res != CL_SUCCESS)
        throw CLException();
    context = res_context;
}

void Device::setDefaultController(void)
{
    controller = findVendorController(device);
}

void Device::initialize(cl_device_id clDevice)
{
    cl_platform_id clPlatform = getPlatform(clDevice);
    Controller* const resController = (controller == nullptr)
          ? findVendorController(clDevice)
          : controller;

    device = clDevice;
    platform = clPlatform;
    controller = resController;
    info = controller->getInfo(device);
}

void Device::release(void) noexcept
{
    clReleaseContext(context);
    clReleaseDevice(device);
}
