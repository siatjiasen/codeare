#if defined HAVE_CL_CL_H
    #include <CL/cl.h>
#elif defined HAVE_OPENCL_CL_H
    #include <OpenCL/opencl.h>
#endif


const char *KernelSource = "\n" \
	"__kernel void square(                                                  \n" \
	"   __global float* input,                                              \n" \
	"   __global float* output,                                             \n" \
	"   const unsigned int count)                                           \n" \
	"{                                                                      \n" \
	"   int i = get_global_id(0);                                           \n" \
	"   if(i < count)                                                       \n" \
	"       output[i] = input[i] * input[i];                                \n" \
	"}                                                                      \n" \
	"\n";


bool HaveGPU() {

	int              e;         // return states
	cl_context       ctxt;      // context
	cl_platform_id   cpi;       // platform id
	cl_uint          pfs, devs; // # plattforms / devices
	cl_device_id     did;       // device id
	cl_command_queue cq;        // compute command queue
    cl_program       cp;        // compute program

	e = clGetPlatformIDs (1, &cpi, &pfs);

    if (e != CL_SUCCESS) {
        printf("OpenCL: Failed to create a plattform! No GPU computing.\n"); 
		return false;
	} 
	
	e = clGetDeviceIDs(cpi, CL_DEVICE_TYPE_GPU, 1, &did, &devs);

    if (e != CL_SUCCESS) {
        printf("OpenCL: Failed to create a device group! No GPU computing.\n"); 
		return false;
	}

	if (!(ctxt = clCreateContext (0, 1, &did, NULL, NULL, &e))) {
		printf("OpenCL: Failed to create a compute context! No GPU computing.\n");
		return false;
	}

    if (!(cq = clCreateCommandQueue (ctxt, did, 0, &e))) {
        printf("OpenCL: Failed to create a command! Releasing context. No GPU computing.\n");
		clReleaseContext      (ctxt);
        return false;
    }

    clReleaseCommandQueue (cq);
	
	return true;

}


