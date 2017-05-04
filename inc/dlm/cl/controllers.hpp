#ifndef DLM_CL_DEVICE_CONTROLLERS_HPP_
#define DLM_CL_DEVICE_CONTROLLERS_HPP_
#pragma once
#include "dlm/cl/base.hpp"
#include "dlm/cl/device.hpp"

namespace dlmcl {

class GenericController : public Controller
{
public:
    virtual ~GenericController(void) {};
    virtual DeviceInfo getInfo(cl_device_id device) noexcept;
};

class AMDController : public GenericController
{
public:
    virtual ~AMDController(void) {};
    virtual DeviceInfo getInfo(cl_device_id device) noexcept;
};

class NvidiaController : public GenericController
{
public:
    virtual ~NvidiaController(void) {};
    virtual DeviceInfo getInfo(cl_device_id device) noexcept;
};

class IntelController : public GenericController
{
public:
    virtual ~IntelController(void) {};
    virtual DeviceInfo getInfo(cl_device_id device) noexcept;
};

// needs refactoring: no exception safety, destruction order dependent
extern void registerDefaultVendorControllers(void);

} // ::dlmcl

#endif // DLM_CL_DEVICE_CONTROLLERS_HPP_
