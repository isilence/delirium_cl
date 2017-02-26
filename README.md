## dlmCL

_dlmCl_ is C++ host-side library for OpenCL designed to optimize CPU-GPU data transfers by utilizing features of modern hardware memory architectures.  
Its API provides methods for cross-platform runtime architecture detection and dedicated memory abstraction.  
_dlmCl_ is a support library rather than a complete computing framework, therefore it have low-level primitives that should be used in conjunction with raw OpenCL API.  

### Prerequisites

* C++11 compiler
* OpenCL 1.2 SDK
* CMake 2.6

### Installation

```bash
mkdir build && cd ./build
cmake ../ && make
```

### Example

```c++
// OpenCL objects and kernel params
size_t loc, n;
cl_device_id cl_device;
cl_command_queue queue;
cl_kernel kernel;
...

// initialize device & memory object
dlmcl::Device dev(cl_device);
dlmcl::Memobj mem = dlmcl::Memobj::getOptimal(dev, n, CL_MEM_READ_WRITE);

// gather output data
mem->switchToHost(queue);
void* mem_ptr = mem->getHostMemory();
...

// run kernel
mem->switchToDevice(queue);
cl_mem mem_dev = src->getDeviceMemory();
clSetKernelArg(kernel, 0, sizeof mem_dev, &mem_dev);
clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &n, loc, 0, NULL, NULL);
clFinish(queue);

// gather output data
mem->switchToHost(queue);
void* ptr = mem->getHostMemory();
```

### Contributors

Pavel Begunkov (asml.silence@gmail.com)

