#include <iostream>
#include <stdlib.h>
#include "utils.hpp"
using namespace dlmcl;

const char* attributeNames[] = { "Name", "Vendor", "Version", "Profile"}; // , "Extensions" };
const cl_platform_info attributeTypes[5] = { CL_PLATFORM_NAME, CL_PLATFORM_VENDOR, CL_PLATFORM_VERSION, CL_PLATFORM_PROFILE, CL_PLATFORM_EXTENSIONS };
int attributeCount = sizeof(attributeNames)/sizeof(attributeNames[0]);

void printDeviceInfo(Device& device) {

    for (int j = 0; j < attributeCount; j++) {
        // get platform attribute value size
        size_t infoSize;
        clGetPlatformInfo(device.platform, attributeTypes[j], 0, NULL, &infoSize);
        char* info = (char*) malloc(infoSize);

        // get platform attribute value
        clGetPlatformInfo(device.platform, attributeTypes[j], infoSize, info, NULL);

        printf(" .%d %-11s: %s\n", j+1, attributeNames[j], info);
        free(info);
    }
}

