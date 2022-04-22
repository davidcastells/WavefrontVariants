/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OCLUtils.cpp
 * Author: dcr
 * 
 * Created on December 15, 2021, 8:41 PM
 */

#include "OCLUtils.h"
#include "PerformanceLap.h"

#include <unistd.h>
#include <stdio.h>

extern int verbose;

OCLUtils::OCLUtils()
{
    m_platformId = NULL;
    m_deviceId = NULL;
}


OCLUtils::~OCLUtils()
{
}

OCLUtils m_instance;

OCLUtils* OCLUtils::getInstance()
{
    return &m_instance;
}

void OCLUtils::selectPlatform(cl_uint selected_platform_index)
{
    using namespace std;

    cl_uint num_of_platforms = 0;
    // get total number of available platforms:
    cl_int err = clGetPlatformIDs(0, 0, &num_of_platforms);
    CHECK_CL_ERRORS(err);

//    printf("Num platforms: %d\n", num_of_platforms);
//    fflush(stdout);
        
    // use vector for automatic memory management
    vector<cl_platform_id> platforms(num_of_platforms);
    // get IDs for all platforms:
    err = clGetPlatformIDs(num_of_platforms, &platforms[0], 0);
    CHECK_CL_ERRORS(err);


    bool by_index = true;


    printf(" Platforms (%d):\n", num_of_platforms);
    fflush(stdout);

    // TODO In case of empty platform name select the default platform or 0th platform?

    for (cl_uint i = 0; i < num_of_platforms; ++i)
    {
            // Get the length for the i-th platform name
            size_t platform_name_length = 0;
            err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 0, 0, &platform_name_length);
            CHECK_CL_ERRORS(err);

            // Get the name itself for the i-th platform
            // use vector for automatic memory management
            char platform_name[platform_name_length];
            err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, platform_name_length, platform_name, 0 );
            CHECK_CL_ERRORS(err);

            printf("    [%d] %s", i,  platform_name);

            // decide if this i-th platform is what we are looking for
            // we select the first one matched skipping the next one if any
            //
            if (selected_platform_index == i || // we already selected the platform by index
                //string(&platform_name[0]).find(required_platform_subname) != string::npos &&
                selected_platform_index == num_of_platforms // haven't selected yet
                )
            {
                    printf(" [Selected]");
                    selected_platform_index = i;
                    // do not stop here, just want to see all available platforms
            }

            // TODO Something when more than one platform matches a given subname

            printf("\n");
            fflush(stdout);
    }

    if (by_index && selected_platform_index >= num_of_platforms)
    {
            throw std::runtime_error(
                    "Given index of platform (" + to_string(selected_platform_index) + ") "
                    "is out of range of available platforms"
            );
    }

    /*if (!by_index && selected_platform_index >= num_of_platforms)
    {
            throw Error(
                    "There is no found platform with name containing \"" +
                    required_platform_subname + "\" as a substring\n"
            );
    }*/

    m_platformId = platforms[selected_platform_index];
}


void  OCLUtils::selectDevice(cl_uint selected_device_index)
{
    using namespace std;

    // List devices of a given type only
    cl_device_type device_type = CL_DEVICE_TYPE_ALL; //  = parseDeviceType(device_type_name);

    cl_uint num_of_devices = 0;
    cl_int err = clGetDeviceIDs(m_platformId, device_type, 0, 0, &num_of_devices );

    CHECK_CL_ERRORS(err);

    vector<cl_device_id> devices(num_of_devices);

    m_deviceId = 0;

    err = clGetDeviceIDs(m_platformId, device_type, num_of_devices, &devices[0], 0 );
    CHECK_CL_ERRORS(err);

    printf("Devices (%d):\n",  num_of_devices);

    for (cl_uint i = 0; i < num_of_devices; ++i)
    {
        // Get the length for the i-th device name
        size_t device_name_length = 0;
        err = clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 0, 0, &device_name_length );
        CHECK_CL_ERRORS(err);

        // Get the name itself for the i-th device
        // use vector for automatic memory management
        char device_name[device_name_length];
        err = clGetDeviceInfo( devices[i], CL_DEVICE_NAME, device_name_length, device_name, 0 );
        CHECK_CL_ERRORS(err);

        printf("    [%d] %s", i, device_name);

        if (i == selected_device_index)
        {
            printf(" [Selected]");
            m_deviceId = devices[i];
        }

        printf("\n");
        fflush(stdout);
    }
}


std::string OCLUtils::errorToStr(cl_int error)
{
#define CASE_CL_CONSTANT(NAME) case NAME: return #NAME;

	// Suppose that no combinations are possible.
	// TODO: Test whether all error codes are listed here
	switch (error)
	{
		CASE_CL_CONSTANT(CL_SUCCESS)
			CASE_CL_CONSTANT(CL_DEVICE_NOT_FOUND)
			CASE_CL_CONSTANT(CL_DEVICE_NOT_AVAILABLE)
			CASE_CL_CONSTANT(CL_COMPILER_NOT_AVAILABLE)
			CASE_CL_CONSTANT(CL_MEM_OBJECT_ALLOCATION_FAILURE)
			CASE_CL_CONSTANT(CL_OUT_OF_RESOURCES)
			CASE_CL_CONSTANT(CL_OUT_OF_HOST_MEMORY)
			CASE_CL_CONSTANT(CL_PROFILING_INFO_NOT_AVAILABLE)
			CASE_CL_CONSTANT(CL_MEM_COPY_OVERLAP)
			CASE_CL_CONSTANT(CL_IMAGE_FORMAT_MISMATCH)
			CASE_CL_CONSTANT(CL_IMAGE_FORMAT_NOT_SUPPORTED)
			CASE_CL_CONSTANT(CL_BUILD_PROGRAM_FAILURE)
			CASE_CL_CONSTANT(CL_MAP_FAILURE)
			CASE_CL_CONSTANT(CL_MISALIGNED_SUB_BUFFER_OFFSET)
			CASE_CL_CONSTANT(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST)
			CASE_CL_CONSTANT(CL_INVALID_VALUE)
			CASE_CL_CONSTANT(CL_INVALID_DEVICE_TYPE)
			CASE_CL_CONSTANT(CL_INVALID_PLATFORM)
			CASE_CL_CONSTANT(CL_INVALID_DEVICE)
			CASE_CL_CONSTANT(CL_INVALID_CONTEXT)
			CASE_CL_CONSTANT(CL_INVALID_QUEUE_PROPERTIES)
			CASE_CL_CONSTANT(CL_INVALID_COMMAND_QUEUE)
			CASE_CL_CONSTANT(CL_INVALID_HOST_PTR)
			CASE_CL_CONSTANT(CL_INVALID_MEM_OBJECT)
			CASE_CL_CONSTANT(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)
			CASE_CL_CONSTANT(CL_INVALID_IMAGE_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_SAMPLER)
			CASE_CL_CONSTANT(CL_INVALID_BINARY)
			CASE_CL_CONSTANT(CL_INVALID_BUILD_OPTIONS)
			CASE_CL_CONSTANT(CL_INVALID_PROGRAM)
			CASE_CL_CONSTANT(CL_INVALID_PROGRAM_EXECUTABLE)
			CASE_CL_CONSTANT(CL_INVALID_KERNEL_NAME)
			CASE_CL_CONSTANT(CL_INVALID_KERNEL_DEFINITION)
			CASE_CL_CONSTANT(CL_INVALID_KERNEL)
			CASE_CL_CONSTANT(CL_INVALID_ARG_INDEX)
			CASE_CL_CONSTANT(CL_INVALID_ARG_VALUE)
			CASE_CL_CONSTANT(CL_INVALID_ARG_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_KERNEL_ARGS)
			CASE_CL_CONSTANT(CL_INVALID_WORK_DIMENSION)
			CASE_CL_CONSTANT(CL_INVALID_WORK_GROUP_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_WORK_ITEM_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_GLOBAL_OFFSET)
			CASE_CL_CONSTANT(CL_INVALID_EVENT_WAIT_LIST)
			CASE_CL_CONSTANT(CL_INVALID_EVENT)
			CASE_CL_CONSTANT(CL_INVALID_OPERATION)
			CASE_CL_CONSTANT(CL_INVALID_GL_OBJECT)
			CASE_CL_CONSTANT(CL_INVALID_BUFFER_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_MIP_LEVEL)
			CASE_CL_CONSTANT(CL_INVALID_GLOBAL_WORK_SIZE)
//			CASE_CL_CONSTANT(CL_INVALID_PROPERTY)
//			CASE_CL_CONSTANT(CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR)
	default:
		return "UNKNOWN ERROR CODE " + error;
	}

#undef CASE_CL_CONSTANT
}

void pfn_notify(const char *errinfo, const void *private_info, size_t cb, void *user_data)
{
    fprintf(stdout, "[OCL] OpenCL Error (via pfn_notify): %s\n", errinfo);
    fprintf(stdout, "[OCL] private info: %p\n", private_info);
    fprintf(stdout, "[OCL] user data: %p\n", user_data);
    fflush(stdout);
}

void build_notify(cl_program program, void* user_data)
{
    fprintf(stdout, "[OCL] Build notify (via pfn_notify):\n");
    fprintf(stdout, "[OCL] user data: %p\n", user_data);
    fflush(stdout);
}

void OCLUtils::releaseContext(cl_context context)
{   
    cl_int err;
    err = clReleaseContext(context);
    CHECK_CL_ERRORS(err);
}

void OCLUtils::releaseMemObject(cl_mem& p)
{
    cl_int err;
    
    if (p != NULL)
        err = clReleaseMemObject(p);
    
    p = NULL;
    CHECK_CL_ERRORS(err);
}

/**
 * Creates a context a sets it as the default context
 * @return 
 */
cl_context OCLUtils::createContext()
{
    using namespace std;

    if (!m_platformId)
        throw runtime_error("Platform is not selected");
    if (!m_deviceId)
        throw runtime_error("Device is not selected");

    size_t number_of_additional_props = 0;
    /*if (additional_context_props)
    {
            // count all additional props including terminating 0
            while (additional_context_props[number_of_additional_props++]);
            number_of_additional_props--;   // now exclude terminating 0
    }*/

    // allocate enough space for platform and all additional props if any
    std::vector<cl_context_properties> context_props(
            2 + // for CL_CONTEXT_PLATFORM and platform itself
            number_of_additional_props +
            1   // for terminating zero
    );

    context_props[0] = CL_CONTEXT_PLATFORM;
    context_props[1] = cl_context_properties(m_platformId);
    /*
    std::copy(
            additional_context_props,
            additional_context_props + number_of_additional_props,
            context_props.begin() + 2   // +2 -- skipping already initialized platform entries
    );*/

    context_props.back() = 0;

    cl_int err = 0;
    cl_context context = clCreateContext(&context_props[0], 1, &m_deviceId, pfn_notify, 0, &err);
    CHECK_CL_ERRORS(err);

    m_context = context;

    return context;
}

int OCLUtils::fileExists(const char *file_name) 
{
#ifdef _WIN32 // Windows
  DWORD attrib = GetFileAttributesA(file_name);
  return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
#else         // Linux
  return access(file_name, R_OK) != -1;
#endif
}

std::string OCLUtils::loadSourceFile(const char* filename)
{
    // Read program file and place content into buffer 
    FILE* fp = fopen(filename, "r");

    if (fp == NULL) 
        throw std::runtime_error(std::string("File not found") + std::string(filename));

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
   
    char* buffer = (char*)malloc(size + 1);

    fread(buffer, sizeof(char), size, fp);
    buffer[size] = '\0';

    fclose(fp);

    
    std::string ret(buffer);
    
    free(buffer);
    
    return ret;
}

cl_program OCLUtils::createProgramFromSource(const char* sourceFile)
{
    printf("Loading %s\n", sourceFile);
    cl_program program;
      
    std::string source = loadSourceFile(sourceFile);
    
    if (verbose)
    {
        printf("SOURCE:\n");
        printf("-------------------------------\n");
        printf(source.c_str());
        printf("\n-------------------------------\n");
        printf("\n");
    }
    const char* sources[] = {source.c_str()};
    const size_t sourcesLen[] = {source.size()};
    
    cl_int err;
    program = clCreateProgramWithSource(m_context, 1, sources, sourcesLen, &err);
    CHECK_CL_ERRORS(err);
    
    cl_device_id device_ids[] = {m_deviceId};
    
    const char* options = "-cl-nv-verbose"; //  -cl-opt-disable";
    
    PerformanceLap lap;
    err = clBuildProgram(program, 1, device_ids, options, build_notify, NULL);
//    CHECK_CL_ERRORS(err);
    
    // Determine the size of the log
    size_t log_size;
    err = clGetProgramBuildInfo(program, m_deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    CHECK_CL_ERRORS(err);

    // Allocate memory for the log
    char *log = (char *) malloc(log_size);

    // Get the log
    err = clGetProgramBuildInfo(program, m_deviceId, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
    CHECK_CL_ERRORS(err);
    
    // Print the log
    printf("BUILD INFO:\n");
    printf(log);
    printf("\n");
    free(log);
        
    lap.stop();
    
    printf("Kernel compilation time: %0.2f seconds\n", lap.lap());
    
    m_program = program;
    
    return program;
}

cl_kernel OCLUtils::createKernel(const char* name)
{
    cl_int ret;
    cl_kernel kernel = clCreateKernel(m_program, name, &ret);
    CHECK_CL_ERRORS(ret);
    
    return kernel;
}

//cl_program OCLUtils::createProgramFromBinary(const char *binary_file_name) 
//{
//    printf("Loading %s...\n", binary_file_name);
//    printf("For %d devices...\n", num_devices);
//    
//  // Early exit for potentially the most common way to fail: AOCX does not exist.
//  if(!fileExists(binary_file_name)) 
//  {
//    printf("AOCX file '%s' does not exist.\n", binary_file_name);
//    throw Error("Failed to load binary file");
//  }
//
//  // Load the binary.
//  size_t binary_size;
//  unsigned char* binary = loadBinaryFile(binary_file_name, &binary_size);
//  
//  if (binary == NULL) 
//  {
//      throw Error("Failed to load binary file");
//  }
//
//  size_t binary_lengths[num_devices];
//  unsigned char * binaries[num_devices];
//  
//  for (int i = 0; i < num_devices; i++) 
//  {
//    binary_lengths[i] = binary_size;
//    binaries[i] = binary;
//  }
//
//  cl_int status;
//  scoped_array<cl_int> binary_status(num_devices);
//
//  cl_program program = clCreateProgramWithBinary(context, num_devices, devices, binary_lengths,
//      (const unsigned char **) binaries, binary_status, &status);
//
//  printf("create program finished\n");
//  fflush(stdout);
//  //checkError(status, "Failed to create program with binary");
//  SAMPLE_CHECK_ERRORS(status);
////  for(unsigned i = 0; i < num_devices; ++i) {
////    throw Error("Failed to load binary for device");
////  }
//
//  return program;
//}

OCLQueue* OCLUtils::createQueue()
{
    using namespace std;

    //cl_queue_properties queue_properties = 0;
    
    if (!m_deviceId)
        throw runtime_error("Device is not selected");

    cl_int err = 0;
    cl_command_queue queue = clCreateCommandQueueWithProperties(m_context, m_deviceId, NULL, &err);
    CHECK_CL_ERRORS(err);
    
    return new OCLQueue(queue);
}

OCLQueue::OCLQueue(cl_command_queue queue)
{
    m_queue = queue;
}

OCLQueue::~OCLQueue()
{
    cl_int err = clReleaseCommandQueue(m_queue);
    // CHECK_CL_ERRORS(err);
}

void OCLQueue::invokeKernel1D(cl_kernel kernel, size_t workitems)
{
    size_t wgSize[3] = {1, 1, 1};
    size_t gSize[3] = {workitems, 1, 1};

    cl_int ret = clEnqueueNDRangeKernel(m_queue, kernel, 1, NULL, gSize, wgSize, 0, NULL, NULL);
    CHECK_CL_ERRORS(ret);
}

void OCLQueue::writeBuffer(cl_mem buf, void* dst, size_t size )
{
    cl_int ret;
    ret = clEnqueueWriteBuffer(m_queue, buf, CL_TRUE, 0, size, dst, 0, NULL, NULL);
    CHECK_CL_ERRORS(ret);
}

void OCLQueue::readBuffer(cl_mem buf, void* dst, size_t size )
{
    cl_int ret;
    ret = clEnqueueReadBuffer(m_queue, buf, CL_TRUE, 0, size, dst, 0, NULL, NULL);
    CHECK_CL_ERRORS(ret);
}
