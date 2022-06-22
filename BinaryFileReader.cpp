#include "BinaryFileReader.h"

#include <stdio.h>
#include <string>
#include <stdexcept>


char* BinaryFileReader::read(const char* file, long* size)
{
  FILE* fp;
  fp = fopen(file, "rb");
  
  if(fp == 0)
      throw std::runtime_error(std::string("File not found ") + std::string(file));

  // Get the size of the file
  fseek(fp, 0, SEEK_END);
  *size = ftell(fp);

  size_t total = *size;
  int offset = 0;
  int nread = 0;

  // Allocate space for the binary
  unsigned char *binary = new unsigned char[total];

  printf("Binary File Size: %ld\n",  total);
  
  // Go back to the file start
  //fseek(fp, 0, SEEK_SET);
  rewind(fp);

  
  if (fread((void*)binary, 1, total, fp) < total)
    {
        delete[] binary;
        fclose(fp);
        throw std::runtime_error(std::string("### ERROR ### could not load binary"));
    }
  
  printf("Binary Read: [OK]\n");

    
  return binary;
}
