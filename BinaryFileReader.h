#ifndef BINARY_FILE_READER_H_INCLUDED_
#define BINARY_FILE_READER_H_INCLUDED_

#include <stdlib.h>

class BinaryFileReader
{
public:
    static char* read(const char* file, long* size);
};


#endif
