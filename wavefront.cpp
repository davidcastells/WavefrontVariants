#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>

#include "PerformanceLap.h"
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
#include "OCLUtils.h"
#include "OCLGPUWavefrontOriginal2Cols.h"
#include "OCLFPGAWavefrontOriginal2Cols.h"

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


int doDP = 0;
int doDP2 = 0;
int doWFO = 0;
int doWFO2 = 0;
int doWFO2OCL = 0;
int doWFE = 0;
int doWFD = 0;
int doWFD2 = 0;
int doWFDD = 0;
int doWFDD2 = 0;
int doAlignmentPath = 0;

long gM = 100;
long gN = 100;
long gK = 100;

char* gP = NULL;
char* gT = NULL;

char* gfP = NULL;
char* gfT = NULL;

int gPid = 0;           // OpenCL platform ID
int gDid = 0;           // OpenCL device ID
int gWorkgroupSize = 8; // Workgroup size

int verbose = 0;

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
//            printf("gpid = %d\n", gPid);
        }
        if ((strcmp(args[i], "-did") == 0) || (strcmp(args[i], "--opencl-device-id") == 0))
        {
            gDid = atol(args[++i]);
        }
        if ((strcmp(args[i], "-wgs") == 0) || (strcmp(args[i], "--workgroup-size") == 0))
        {
            gWorkgroupSize = atol(args[++i]);
        }
                
        if (strcmp(args[i], "-P") == 0)
            gP = args[++i];
        if (strcmp(args[i], "-T") == 0)
            gT = args[++i];
        if (strcmp(args[i], "-fP") == 0)
            gfP = args[++i];
        if (strcmp(args[i], "-fT") == 0)
            gfT = args[++i];
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
        if ((strcmp(args[i], "-v") == 0) || (strcmp(args[i], "--verbose") == 0))
            verbose = 1;
        if ((strcmp(args[i], "-vv") == 0) || (strcmp(args[i], "--verbose-2") == 0))
            verbose = 2;
        if ((strcmp(args[i], "-h") == 0) || (strcmp(args[i], "--help") == 0))
            usage();
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
    printf("    %s-DP%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Use the dynamic programming approach with full table (no wavefront)\n");
    printf("    %s-DP2%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Use the dynamic programming approach with 2 columns (no wavefront)\n");
    printf("    %s-WFO%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Use the original wavefront approach.\n");
    printf("    %s-WFO2%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Use the original wavefront approach with 2 columns.\n");
    printf("    %s-WFO2OCL%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Use the original wavefront approach with 2 columns in OpenCL.\n");
    printf("    %s-WFE%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Use the wavefront approach with extend table precomputation.\n");
    printf("    %s-WFD%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Use the wavefront diamond approach.\n");
    printf("    %s-WFDD%s\n", TEXT_SCAPE_BOLD, TEXT_SCAPE_END, TEXT_SCAPE_UNDERLINE, TEXT_SCAPE_END);
    printf("        Use the wavefront dynamic diamond approach.\n");
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

    exit(0);
}


int main(int argc, char* args[])
{
    parseArgs(argc, args);

    FastaInfo fastaP;
    FastaInfo fastaT;

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
            printf("Reading input files\n");
            fastaP = FastaReader::read(gfP);
            fastaT = FastaReader::read(gfT);

            gP = fastaP.seq;
            gT = fastaT.seq;

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
    printf("m=%d n=%d k=%d\n", gM, gN, gK);

//        printf("do alignment: %d\n", doAlignmentPath);


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

    return 0;
}





