/* http://opencl.codeplex.com/wikipage?title=OpenCL%20Tutorials%20-%201 */
#include <stdio.h>
#include <CL/opencl.h>

void main(){
  cl_int error = 0;   // Used to handle error codes
  cl_platform_id platform;
  cl_context context;
  cl_command_queue queue;
  cl_device_id device;

  // Platform
  error = oclGetPlatformID(&platform);
  if (error != CL_SUCCESS) {
    printf("Error getting platform id: %s\n",
           errorMessage(error));
    exit(error);
  }
  // Device
  error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
  if (error != CL_SUCCESS) {
    printf("Error getting device ids: %s\n",
           errorMessage(error));
    exit(error);
  }
  // Context
  context = clCreateContext(0, 1, &device, NULL, NULL, &error);
  if (error != CL_SUCCESS) {
    printf("Error creating context: %s\n",
           errorMessage(error));
    exit(error);
  }
  // Command-queue
  queue = clCreateCommandQueue(context, device, 0, &error);
  if (error != CL_SUCCESS) {
    printf("Error creating command queue: %s\n",
           errorMessage(error));
    exit(error);
  }
}
