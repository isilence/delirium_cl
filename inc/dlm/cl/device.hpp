#ifndef DLM_CL_DEVICE_HPP_
#define DLM_CL_DEVICE_HPP_
#pragma once
#include "dlm/cl/base.hpp"

namespace dlmcl {

// ======================================
//          Controller class
// ======================================

enum OPTIMIZATION_LEVEL {
    OPT_O1 = 0,
    OPT_O2 = 1,
    OPT_O3 = 2,
    OPT_MAX = OPT_O3
};

class Controller
{
public:
    virtual ~Controller(void) {};
    virtual DeviceInfo getInfo(cl_device_id device) noexcept = 0;
    virtual const char* getCompilationOptions(cl_device_id device, enum OPTIMIZATION_LEVEL) noexcept = 0;
};

// ======================================
//          Device class
// ======================================

class Device
{
private:
    Controller* controller;

    void initialize(cl_device_id device);
    void release(void) noexcept;

public:
    static const int MAX_VENDOR_NAME = 50;
    static const char DEFAULT_VENDOR[MAX_VENDOR_NAME];

    static void         getVendorName(cl_device_id device, char* buffer, size_t size);
    static int          registerVendorController(const char *vendorName, Controller *vendor)    noexcept;
    static Controller*  getVendorController(const char *vendorName)                             noexcept;

    explicit Device(cl_device_id device);
    explicit Device(cl_device_id device, Controller* controller);
    explicit Device(Device&&)   = default;
    Device(void)                = delete;
    Device(const Device&)       = delete;
    ~Device(void)               noexcept;

    void    setDefaultContext(void);
    void    setContext(cl_context context) noexcept;
    void    setDefaultController(void);
    void    setController(Controller* controller) noexcept;

    const char* getCompilationOptions(enum OPTIMIZATION_LEVEL) noexcept;

    cl_device_id    device;
    cl_platform_id  platform;
    cl_context      context;
    DeviceInfo      info;
};

inline Device::Device(cl_device_id device)
    : controller(nullptr)
{
    initialize(device);
    setDefaultContext();
}

inline Device::Device(cl_device_id device, Controller* controller)
    : controller(controller)
{
    initialize(device);
    setDefaultContext();
}

inline void Device::setController(Controller* newController) noexcept
{
    controller = newController;
}

inline Device::~Device(void) noexcept
{
    release();
}

inline void Device::setContext(cl_context new_context) noexcept
{
    context = new_context;
}

inline const char* Device::getCompilationOptions(enum OPTIMIZATION_LEVEL level) noexcept
{
    return controller->getCompilationOptions(device, level);
}

} // ::dlmcl
#endif // DLM_CL_DEVICE_HPP_
