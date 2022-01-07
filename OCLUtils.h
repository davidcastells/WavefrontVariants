/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OCLUtils.h
 * Author: dcr
 *
 * Created on December 15, 2021, 8:41 PM
 */

#ifndef OCLUTILS_H
#define OCLUTILS_H

#include <CL/cl.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

#define CHECK_CL_ERRORS(ERR)                        \
    if(ERR != CL_SUCCESS)                               \
    {                                                   \
        throw std::runtime_error(                                    \
            std::string("OpenCL error ") +                           \
            OCLUtils::errorToStr(ERR)                   \
			+ std::string(" happened in file ") \
			+ std::string((const char*)(__FILE__))    \
			+ std::string(" at line ") + std::to_string(__LINE__)  \
        );                                              \
    }

class OCLQueue;

/**
 * OpenCL utils
 */
class OCLUtils {
public:
    OCLUtils();
    virtual ~OCLUtils();

    static OCLUtils* getInstance();
    
    void selectPlatform(cl_uint selected_platform_index);
    void selectDevice(cl_uint selected_device_index);
    cl_context createContext();
    void releaseMemObject(cl_mem& p);
    void releaseContext(cl_context context);
    OCLQueue* createQueue();
    cl_program createProgramFromSource(const char* sourceFile, std::string& options);
    cl_program createProgramFromBinary(const char * binaryFile );
    
    cl_kernel createKernel(const char* name);
    
    // File Utils
    static int fileExists(const char *file_name);
    static std::string getDir(const char* file_name);
    std::string loadSourceFile(const char* filename);
    unsigned char* loadBinaryFile(const char* filename, size_t *size);
    void setKernelDir(std::string& file);
    
    // String Utils
    static int contains(std::string& str, const char* q);
    static std::string errorToStr(cl_int err);
    
    std::string getSelectedPlatformName() { return m_selectedPlatformName; }
    
private:
    cl_platform_id m_platformId;
    cl_device_id m_deviceId;
    cl_context m_context;
    cl_program m_program;
    std::vector<cl_platform_id> m_platforms;
    
    std::string m_selectedPlatformName;
    
    std::string m_kernelDir;
};

class OCLQueue
{
public:
    OCLQueue(cl_command_queue queue);
    virtual ~OCLQueue();
    
    void invokeKernel1D(cl_kernel kernel, size_t workitems, size_t workgroupsize);
    void writeBuffer(cl_mem buf, void* src, size_t size );
    void readBuffer(cl_mem buf, void* dst, size_t size);

public:
    cl_command_queue m_queue;
};

#endif /* OCLUTILS_H */

