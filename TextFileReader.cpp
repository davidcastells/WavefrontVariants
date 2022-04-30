/*
 fasta.c
 Functions for module fasta 
 $Id: fasta.c,v 1.0 2007/04/03/ 09:46 jianhua Exp $
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<assert.h>
#include<math.h>
#include <map>
#include <algorithm>
#include <ios>
#include <iostream>
#include <string>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <vector>
using namespace std;

#include "TextFileReader.h"


char* TextFileReader::read(const char* file)
{
    printf("Reading %s\n", file);

    FILE* fp = fopen(file, "r");

    if (fp == NULL)
        throw std::runtime_error("File not found");

    if (fseek(fp, 0, SEEK_END) == -1)
        throw std::runtime_error("failed to seek to end");
    
    
    size_t size = ftell(fp);
    
    char* ret = (char*) malloc(size);
    
    if (fseek(fp, 0, SEEK_SET) == -1)
        throw std::runtime_error("failed to seek to start");
        
    size_t read = fread(ret, sizeof(char), size, fp );
    
    if (read < size)
        throw std::runtime_error("could not read all content");
        
    //printf("Read %ld\n", ret.seqLen);
    fclose(fp);

    return ret;
}



