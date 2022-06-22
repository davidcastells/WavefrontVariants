#include "utils.h"
#include <stdexcept>
#include <stdlib.h>
#include <string.h>


bool ends_with(const std::string &filename, const std::string &ext)
{
  return ext.length() <= filename.length() &&
         std::equal(ext.rbegin(), ext.rend(), filename.rbegin());
}

size_t strnlen(char* s, size_t max)
{
    for (size_t i=0; i < max; i++)
        if (s[i] == 0) return i;
}

#define min(a, b) ((a)>(b))? (b):(a)

bool file_is_binary(char* filename)
{
  FILE* fp;
  fp = fopen(filename, "rb");
  
  if(fp == 0)
      throw std::runtime_error(std::string("File not found ") + std::string(filename));

  // Get the size of the file
  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);

  char buffer [1024*32];
  
  // Go back to the file start
  //fseek(fp, 0, SEEK_SET);
  rewind(fp);

  size_t total = size;
  
  do
  {
    size_t toread = min(total, 1024*32);
    size_t read = fread((void*)buffer, 1, toread, fp);
    
    if (strnlen((char*) buffer, toread) < read)
    {
        fclose(fp);
        // this is a binary
        return true;
    }
    
    total -= read;
  } while (total > 0);
  
  fclose(fp);
    
  return false;

}

