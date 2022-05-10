/**
 * 
 * Copyright (C) 2021, David Castells-Rufas <david.castells@uab.cat>, 
 * Universitat Autonoma de Barcelona  
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>
#include <stdexcept>

#include "PerformanceLap.h"
#include "EdlibAligner.h"
#include "LevDP.h"
#include "LevDP2Cols.h"
#include "WavefrontOriginal.h"
#include "WavefrontOriginal2Cols.h"
#include "WavefrontExtendPrecomputing.h"
#include "WavefrontDiamond.h"
#include "WavefrontDiamond2Cols.h"
#include "WavefrontDynamicDiamond.h"
#include "WavefrontDynamicDiamond2Cols.h"
#include "utils.h"
#include "fasta.h"
#include "TextFileReader.h"
#include "OCLUtils.h"
#include "OCLGPUWavefrontOriginal2Cols.h"
#include "OCLFPGAWavefrontOriginal2Cols.h"
#include "OCLGPUWavefrontDynamicDiamond2Cols.h"
#include "CUDAWavefrontOriginal2Cols.h"
#include "CUDAWavefrontDynamicDiamond2Cols.h"

int wavefront_classic(const char* P, const char* T, int m, int n, int k);
int wavefront_dcr(const char* P, const char* T, int m, int n, int k);

void test_primitives();
int polarExistsInD(int d, int r);
int polarExistsInW(int d, int r);


void generatePT(char** pP, char** pT, int m, int n)
{
	char s[]="ACGT";
	
	*pP = new char[m+1];
	*pT = new char[n+1];
	
	assert(*pP);
	assert(*pT);
	
	for (int i=0; i< m; i++)
		(*pP)[i] = s[rand()%4];
	(*pP)[m] = 0;
	
	for (int i=0; i< n; i++)
		(*pT)[i] = s[rand()%4];
	(*pT)[n] = 0;
	
	for (int k=0; k < 3; k++)
	{
            // now copy fragments of P in T, to reduce errors
            int lenCopy = (rand() % min2(m,n)) / 1.3;
            int startP = rand() % (m-lenCopy);
            int startT = startP + (rand() % (200));


            printf("copy fragment %d\n", lenCopy);
            for (int i=0; i < lenCopy; i++)
                if (startT+i < n)
                    (*pT)[startT+i] = (*pP)[startP+i]; 
	}
}

int doED = 0;               // Edlib
int doDP = 0;
int doDP2 = 0;
int doWFO = 0;
int doWFO2 = 0;
int doWFO2OCL = 0;
int doWFO2CUDA = 0;
int doWFE = 0;
int doWFD = 0;
int doWFD2 = 0;
int doWFDD = 0;
int doWFDD2 = 0;
int doWFDD2OCL = 0;
int doWFDD2CUDA = 0;

int doAlignmentPath = 0;

INT_TYPE gM = 100;          // Pattern sequence length
INT_TYPE gN = 100;          // Test sequence length
INT_TYPE gK = 100;          // Maximum allowed error

char* gP = NULL;
char* gT = NULL;

char* gfP = NULL;
char* gfT = NULL;

int gPid = 0;           // OpenCL platform ID
int gDid = 0;           // OpenCL device ID
int gWorkgroupSize = 8; // Workgroup size

double gPrintPeriod = -1;

int gExtendAligned = 0; // use aligned reads during extension

int gMeasureIterationTime = 0;

int verbose = 0;
int gStats = 0;

int gEnqueuedInvocations = 100;
int gTileLen = 3;                       // Tile length
char* gLocalTileMemory = "register";    // can be register or shared

std::string gExeDir = ".";

void usage();

void parseArgs(int argc, char* args[])
{
    gExeDir = OCLUtils::getDir(args[0]);
    
    auto ocl = OCLUtils::getInstance();
    ocl->setKernelDir(gExeDir);
    if (verbose)
        printf("kernel dir: %s\n", gExeDir.c_str());
    
    for (int i=1; i < argc; i++)
    {
        //printf("parsing %s\n", args[i]);

        if (strcmp(args[i], "-m") == 0)
        {
            gM = atol(args[++i]);
        }
        if (strcmp(args[i], "-n") == 0)
        {
            gN = atol(args[++i]);
        }
        if (strcmp(args[i], "-k") == 0)
        {
            gK = atol(args[++i]);
        }

        if ((strcmp(args[i], "-a") == 0) || (strcmp(args[i], "--align") == 0))
        {
            doAlignmentPath = 1;
        }
        if ((strcmp(args[i], "-pid") == 0) || (strcmp(args[i], "--opencl-platform-id") == 0))
        {
            gPid = atol(args[++i]);
        }
        if ((strcmp(args[i], "-did") == 0) || (strcmp(args[i], "--opencl-device-id") == 0))
        {
            gDid = atol(args[++i]);
        }
        if ((strcmp(args[i], "-wgs") == 0) || (strcmp(args[i], "--workgroup-size") == 0))
        {
            gWorkgroupSize = atol(args[++i]);
        }
        if ((strcmp(args[i], "-pp") == 0) || (strcmp(args[i], "--print-period") == 0))
        {
            gPrintPeriod = atof(args[++i]);
        }
        if ((strcmp(args[i], "-qi") == 0) || (strcmp(args[i], "--enqueued-invocations") == 0))
        {
            gEnqueuedInvocations = atoi(args[++i]);
        }
        if ((strcmp(args[i], "-tl") == 0) || (strcmp(args[i], "--tile-length") == 0))
        {
            gTileLen = atoi(args[++i]);
        }
                
        if (strcmp(args[i], "-P") == 0)
            gP = args[++i];
        if (strcmp(args[i], "-T") == 0)
            gT = args[++i];
        if (strcmp(args[i], "-fP") == 0)
            gfP = args[++i];
        if (strcmp(args[i], "-fT") == 0)
            gfT = args[++i];
            
        if (strcmp(args[i], "-ED") == 0)
            doED = 1;
        if (strcmp(args[i], "-DP") == 0)
            doDP = 1;
        if (strcmp(args[i], "-DP2") == 0)
            doDP2 = 1;
        if (strcmp(args[i], "-WFO") == 0)
            doWFO = 1;
        if (strcmp(args[i], "-WFO2") == 0)
            doWFO2 = 1;
        if (strcmp(args[i], "-WFO2OCL") == 0)
            doWFO2OCL = 1;
        if (strcmp(args[i], "-WFO2CU") == 0)
            doWFO2CUDA = 1;
        if (strcmp(args[i], "-WFE") == 0)
            doWFE = 1;	
        if (strcmp(args[i], "-WFD") == 0)
            doWFD = 1;	
        if (strcmp(args[i], "-WFD2") == 0)
            doWFD2 = 1;	
        if (strcmp(args[i], "-WFDD") == 0)
            doWFDD = 1;
        if (strcmp(args[i], "-WFDD2") == 0)
            doWFDD2 = 1;		
        if (strcmp(args[i], "-WFDD2OCL") == 0)
            doWFDD2OCL = 1;
        if (strcmp(args[i], "-WFDD2CU") == 0)
            doWFDD2CUDA = 1;

        if ((strcmp(args[i], "-v") == 0) || (strcmp(args[i], "--verbose") == 0))
            verbose = 1;
        if ((strcmp(args[i], "-vv") == 0) || (strcmp(args[i], "--verbose-2") == 0))
            verbose = 2;
        if ((strcmp(args[i], "-h") == 0) || (strcmp(args[i], "--help") == 0))
            usage();
        if ((strcmp(args[i], "-s") == 0) || (strcmp(args[i], "--stats") == 0))
            gStats = 1;
        if ((strcmp(args[i], "-ea") == 0) || (strcmp(args[i], "--extend-aligned") == 0))
            gExtendAligned = 1;
        if (strcmp(args[i], "--measure-iteration-time") == 0)
            gMeasureIterationTime = 1;
        if (strcmp(args[i], "--local-tile-memory") == 0)
            gLocalTileMemory = args[++i];
            
    }
}

void usage()
{
    printf("wavefront - An edit alignment tool\n");
    printf("==================================\n");
    printf("    %swavefront%s [OPTIONS]\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("\n");
    printf("%sDESCRIPTION%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("    wavefront - Levenshtein distance computation between a pattern and a text using (some variant)\n");
    printf("                of the wavefront algorithm\n");
    printf("                (c) Copyright 2021 by David Castells-Rufas \n");
    printf("\n");
    printf("%sOPTIONS\n%s", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("    %s-h,--help%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Display the help message.\n");
    printf("    %s-v,--verbose%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Verbose output.\n");
    printf("    %s-vv,--verbose-2%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        More verbose output.\n");
    printf("    %s-pp,--print-period%s %sNUMBER%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Specify the period (in seconds) of printing progress.\n");
    printf("    %s-s,--stats%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Print various statistics.\n");
    printf("\n");

    printf("  %sInput Options:%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("    %s-m%s %sNUMBER%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Specify the pattern length. The pattern will be created randomly.\n");
    printf("    %s-n%s %sNUMBER%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Specify the text length. The text will be created randomly.\n");
    printf("    %s-P%s %sSTRING%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Specify the pattern.\n");
    printf("    %s-T%s %sSTRING%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Specify the text.\n");
    printf("    %s-fP%s %sFILE%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Specify the FASTA file containing the pattern.\n");
    printf("    %s-fT%s %sFILE%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Specify the FASTA file containing the text.\n");
    printf("\n");

    printf("  %sAlignment Method Options:%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("    %s-ED%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Use Edlib Global Alignment (no wavefront)\n");
    printf("    %s-DP%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Use the dynamic programming approach with full table (no wavefront)\n");
    printf("    %s-DP2%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Use the dynamic programming approach with 2 columns (no wavefront)\n");
    printf("    %s-WFO%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Use the original wavefront approach.\n");
    printf("    %s-WFO2%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Use the original wavefront approach with 2 columns.\n");
    printf("    %s-WFO2OCL%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Use the original wavefront approach with 2 columns in OpenCL.\n");
    printf("    %s-WFO2CU%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Use the original wavefront approach with 2 columns in CUDA.\n");
    printf("    %s-WFE%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Use the wavefront approach with extend table precomputation.\n");
    printf("    %s-WFD%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Use the wavefront diamond approach.\n");
    printf("    %s-WFDD%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Use the wavefront dynamic diamond approach.\n");
    printf("    %s-WFDD2%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END );
    printf("        Use the wavefront dynamic diamond approach with 2 columns.\n");
    printf("    %s-WFDD2OCL%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Use the wavefront dynamic diamond approach with 2 columns in OpenCL.\n");
    printf("    %s-WFDD2CU%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Use the wavefront dynamic diamond approach with 2 columns in CUDA.\n");
    printf("\n");

    printf("  %sOperational Options:%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("    %s-k%s %sNUMBER%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Maximum allowed errors (reduce memory usage).\n");
    printf("    %s-a,--align%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Print alignment path to transforms T into P.\n");
    printf("\n");


    printf("  %sOpenCL Options:%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("    %s-pid,--opencl-platform-id%s %sNUMBER%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Index of the OpenCL platform to use (select from the list).\n");
    printf("    %s-did,--opencl-device-id%s %sNUMBER%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Index of the OpenCL device to use (select from the list).\n");
    printf("    %s-wgs,--workgroup-size%s %sNUMBER%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Number of workitems per Workgroup (default is 8).\n");
    printf("\n");

    printf("  %sExperimental and Performance Analysis Options:%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("    %s--measure-iteration-time%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Reports the cummulative time as iterations progress.\n");
    printf("    %s-ea,--extend-aligned%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END);
    printf("        Do extension phase with aligned read operations.\n"); 
    printf("    %s-qi,--enqueued-invocations%s %sNUMBER%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Number of enqueued kernel invocations before checking status.\n");
    printf("    %s-tl,--tile-length%s %sNUMBER%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Tile length.\n");
    printf("    %s--local-tile-memory%s %sSTRING%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Specify the type of memory used in local tiles [register|shared].\n");
    printf("\n");
    
    exit(0);
}


int main(int argc, char* args[])
{
    parseArgs(argc, args);


    if (gP == NULL && gT == NULL && gfP == NULL && gfT == NULL)
    {
            printf("Generating random Input\n");
            generatePT(&gP, &gT, gM, gN);
            
            if (verbose > 1)
            {
                printf("P: %s\n", gP);
                printf("T: %s\n", gT);
            }
    }
    else if (gfP != NULL)
    {
        if (ends_with(gfP, ".fasta"))
	    {
	        // We process Fasta files
	        FastaInfo fastaP;
	        FastaInfo fastaT;
	        
	        printf("Reading input files\n");
            fastaP = FastaReader::read(gfP);
            fastaT = FastaReader::read(gfT);

            gP = fastaP.seq;
            gT = fastaT.seq;


        }
        else
        {
            //printf("Non fasta files not supported yet\n");
            //exit(-1);
            // Other files are just compared as is
            gP = TextFileReader::read(gfP);
            gT = TextFileReader::read(gfT);
        }

        gM = strlen(gP);
        gN = strlen(gT);
    }
    else
    {
            printf("Explicit Input\n");
            gM = strlen(gP);
            gN = strlen(gT);
    }

    if (gK == -1)
        gK = max2(gM, gN);

    //long m = strlen(P);
    //long n = strlen(T);

    //printf("P=%s\n", P);
    //printf("T=%s\n", T);
    printf("Wavefront algorithm test\n");
//        printf("using %d threads\n", omp_get_max_threads());
    printf("m=%ld n=%ld k=%ld\n", gM, gN, gK);

//        printf("do alignment: %d\n", doAlignmentPath);


    if (doED)
    {
        EdlibAligner aligner;
        aligner.execute(gP, gT, gK, doAlignmentPath);
    }
    // Test Dynamic Programming Levenshtein distance
    if (doDP)
    {	
        LevDP aligner;
        aligner.execute(gP, gT, gK, doAlignmentPath);

    }
    // Test Dynamic Programming Levenshtein distance with 2 cols
    if (doDP2)
    {	
        LevDP2Cols aligner;
        aligner.execute(gP, gT, gK, doAlignmentPath);
    }

    // Test Original Wavefront Algorithm 
    if (doWFO)	
    {
        WavefrontOriginal aligner;
        aligner.execute(gP, gT, gK, doAlignmentPath);
    }

    // Test Original Wavefront Algorithm 
    if (doWFO2)	
    {
        WavefrontOriginal2Cols aligner;
        aligner.execute(gP, gT, gK, doAlignmentPath);
    }

    if (doWFO2OCL)
    {
        OCLUtils* ocl = OCLUtils::getInstance();
        ocl->selectPlatform(gPid);
        std::string pn = ocl->getSelectedPlatformName();
        if (OCLUtils::contains(pn, "FPGA") || OCLUtils::contains(pn, "Xilinx"))
        {
            OCLFPGAWavefrontOriginal2Cols aligner;
            aligner.execute(gP, gT, gK, doAlignmentPath);
        }
        else
        {
            OCLGPUWavefrontOriginal2Cols aligner;
            aligner.execute(gP, gT, gK, doAlignmentPath);
        }
    }
#ifdef HAS_CUDA
    if (doWFO2CUDA)
    {
        CUDAWavefrontOriginal2Cols aligner;
        aligner.execute(gP, gT, gK, doAlignmentPath);
    }
#endif
    // Test Extend Precomputing Wavefront
    if (doWFE)
    {
        WavefrontExtendPrecomputing aligner;
        aligner.execute(gP, gT, gK, doAlignmentPath);
    }

    // Test Diamond Wavefront
    if (doWFD)
    {
        WavefrontDiamond aligner;
        aligner.execute(gP, gT, gK, doAlignmentPath);
    }

    // Test Diamond Wavefront 2 Cols
    if (doWFD2)
    {
        WavefrontDiamond2Cols aligner;
        aligner.execute(gP, gT, gK, doAlignmentPath);
    }

    // Test Dynamic Diamond Wavefront
    if (doWFDD)
    {
        WavefrontDynamicDiamond aligner;
        aligner.execute(gP, gT, gK, doAlignmentPath);
    }

    // Test Dynamic Diamond Wavefront 2 Cols
    if (doWFDD2)
    {
        WavefrontDynamicDiamond2Cols aligner;
        aligner.execute(gP, gT, gK, doAlignmentPath);
    }
    
    if (doWFDD2OCL)
    {
        OCLUtils* ocl = OCLUtils::getInstance();
        ocl->selectPlatform(gPid);
        std::string pn = ocl->getSelectedPlatformName();
        if (OCLUtils::contains(pn, "FPGA") || OCLUtils::contains(pn, "Xilinx"))
        {
            throw std::runtime_error("WFDD2OCL FPGA version not implemented yet");
//            OCLFPGAWavefrontOriginal2Cols aligner;
//            aligner.execute(gP, gT, gK, doAlignmentPath);
        }
        else
        {
            OCLGPUWavefrontDynamicDiamond2Cols aligner;
            aligner.execute(gP, gT, gK, doAlignmentPath);
        }
    }
#ifdef HAS_CUDA
    if (doWFDD2CUDA)
    {
        CUDAWavefrontDynamicDiamond2Cols aligner;
        aligner.execute(gP, gT, gK, doAlignmentPath);
    }
#endif

    return 0;
}





