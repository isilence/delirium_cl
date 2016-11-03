#include <iostream>
#include "utils.hpp"
using namespace dlmcl;

const char* attributeNames[] = { "Name", "Vendor", "Version", "Profile"}; // , "Extensions" };
const cl_platform_info attributeTypes[5] = { CL_PLATFORM_NAME, CL_PLATFORM_VENDOR, CL_PLATFORM_VERSION, CL_PLATFORM_PROFILE, CL_PLATFORM_EXTENSIONS };
int attributeCount = sizeof(attributeNames)/sizeof(attributeNames[0]);

void printDeviceInfo(Device& device) {
    size_t infoSize;
    char* info;
    for (int j = 0; j < attributeCount; j++) {
        // get platform attribute value size
        clGetPlatformInfo(device.platform, attributeTypes[j], 0, NULL, &infoSize);
        info = (char*) malloc(infoSize);

        // get platform attribute value
        clGetPlatformInfo(device.platform, attributeTypes[j], infoSize, info, NULL);

        printf(" .%d %-11s: %s\n", j+1, attributeNames[j], info);
        free(info);

    }

    // Extensions
    // get platform attribute value size
    //clGetDeviceInfo(device.device, CL_DEVICE_EXTENSIONS, 0, NULL, &infoSize);
    //info = (char*) malloc(infoSize);

    // get platform attribute value
    // clGetDeviceInfo(clDevice, CL_DEVICE_EXTENSIONS, infoSize, info, NULL);

    // printf(" .%d %-11s: %s\n", 0, "dev ext", info);
    //free(info);
}

// ===========================================================
//  init / release


