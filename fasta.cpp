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

// #include "bioUtils.h"
#include "fasta.h"


FastaInfo FastaReader::read(const char* file)
{
    FastaInfo ret;

    printf("Reading %s\n", file);

    FILE* fp = fopen(file, "r");

    if (fp == NULL)
        throw std::runtime_error("File not found");

    if (readSeq(fp, &ret) == 0)
        printf("ERROR\n");

    //printf("Read %ld\n", ret.seqLen);
    fclose(fp);

    return ret;
}

int readSeq(FILE *fp,FastaInfo *fastaSeq)
/* read fasta sequence */
{
    char *name;
    char delims[] = " \r\t\n";	
    char *seq;
    char *pSeq = NULL;
    int  seqLen;
    char c;  
    long size = 512;
    long firstAllocBufSize = 128*1024;
    char lineBuf[1024];
   
    if (feof(fp)) 
    {
            printf("FEOF!\n");
            return 0;
    }
	
    while(fgets(lineBuf, sizeof(lineBuf), fp) != NULL) 
    {
        if (lineBuf[0]=='>') 
        {
            name = strtok(lineBuf+1, delims);	    
            fastaSeq->seqName = (char *)malloc(strlen(name)+1);
            strcpy(fastaSeq->seqName, name); 
            break;
        }
    }
				
    seq = (char *)malloc(firstAllocBufSize+1);
    seqLen = 0;
    size = firstAllocBufSize;

/* read sequence */	
    for(;;)
    {
            c = fgetc(fp);		
            if (c==EOF || c=='>')
                    break;			
            if (seqLen>=size) 
            {
                    size = size + size;
                    pSeq = seq;
                    seq = (char *) malloc(size);
                    memcpy(seq, pSeq, seqLen);
                    free(pSeq);
                    pSeq = NULL;
                    }
      if (isalpha(c)){
            seq[seqLen] = c;	
            seqLen++;
       }			
      }

seq[seqLen] = '\0';
fastaSeq->seqLen = seqLen;
fastaSeq->seq = seq;

    if (c=='>') {
	ungetc(c,fp);
	}	
    return 1;
}

void freeFasta(FastaInfo *fasta) 
{
    if (fasta != NULL)
    {
        free(fasta->seqName);
        free(fasta->seq);
    }
    fasta = NULL;
}


