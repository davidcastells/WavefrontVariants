#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
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
int doWFE = 0;
int doWFD = 0;
int doWFD2 = 0;
int doWFDD = 0;
int doWFDD2 = 0;

long gM = 100;
long gN = 100;
long gK = 100;

char* gP = NULL;
char* gT = NULL;

char* gfP = NULL;
char* gfT = NULL;

int verbose = 0;

void usage();

void parseArgs(int argc, char* args[])
{
	for (int i=1; i < argc; i++)
	{
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
		if (strcmp(args[i], "-v") == 0)
			verbose = 1;
		if (strcmp(args[i], "-help") == 0)
			usage();
	}
}

void usage()
{
	printf("Levenshtein distance computation between a pattern and a text using [some variant] of the wavefront algorithm\n");
	printf("Usage:\n");
	printf("\twavefront.exe <options>\n");
	printf("\n\nwhere options are :\n");
	printf("\t-m\tpattern length\n");
	printf("\t-n\ttext length\n");
	printf("\t-k\tmaximum allowed errors (reduce memory usage)\n");
	printf("\t-P\tpattern file\n");
	printf("\t-T\ttext file\n");
	printf("\t-DP\ttest the dynamic programming approach with full table (no wavefront)\n");
	printf("\t-DP2\ttest the dynamic programming approach with 2 columns (no wavefront)\n");
	printf("\t-WFO\ttest the original wavefront approach\n");
	printf("\t-WFE\ttest the wavefront approach with extend table precomputation\n");
	printf("\t-WFD\ttest the wavefront diamond approach\n");
	printf("\t-WFDD\ttest the wavefront dynamic diamond approach\n");
	printf("\t-v\tverbose output\n");
	printf("\t-help\tshows this help\n");
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
	printf("m=%d n=%d k=%d\n", gM, gN, gK);


	// Test Dynamic Programming Levenshtein distance
	if (doDP)
	{	
		LevDP levdp;
		levdp.setInput(gP, gT, gK);
		PerformanceLap lap;
		int ed = levdp.getDistance();
		lap.stop();
		
		printf("Dynamic Programming classic Distance=%d Time=%0.5f seconds\n", ed, lap.lap());
	}
	
	// Test Dynamic Programming Levenshtein distance with 2 cols
	if (doDP2)
	{	
		LevDP2Cols levdp2;
		levdp2.setInput(gP, gT, gK);
		PerformanceLap lap;
		int ed = levdp2.getDistance();
		lap.stop();
		
		printf("Dynamic Programming classic Distance=%d Time=%0.5f seconds\n", ed, lap.lap());
	}

	// Test Original Wavefront Algorithm 
	if (doWFO)	
	{
		WavefrontOriginal wavOrig;
		wavOrig.setInput(gP, gT, gK);
		
		PerformanceLap lap;
		int ed = wavOrig.getDistance();
		lap.stop();
		
		printf("Wavefront classic distance=%d. Time: %0.5f seconds\n", ed, lap.lap());
	}
	
	// Test Original Wavefront Algorithm 
	if (doWFO2)	
	{
		WavefrontOriginal2Cols wavOrig;
		wavOrig.setInput(gP, gT, gK);
		
		PerformanceLap lap;
		int ed = wavOrig.getDistance();
		lap.stop();
		
		printf("Wavefront classic 2 cols distance=%d. Time: %0.5f seconds\n", ed, lap.lap());
	}

	// Test Extend Precomputing Wavefront
	if (doWFE)
	{
		WavefrontExtendPrecomputing wavExtend;
		wavExtend.setInput(gP, gT, gK);
		
		PerformanceLap lap;
		wavExtend.precomputeExtend();
		lap.stop();		
		printf("Precomputing Extend Table. Time: %0.5f seconds\n", lap.lap());
		
		lap.start();
		int ed = wavExtend.getDistance();
		lap.stop();
		
		printf("Wavefront precomputed extend distance=%d. Time: %0.5f seconds\n", ed, lap.lap());
	}
	
	// Test Diamond Wavefront
	if (doWFD)
	{
		WavefrontDiamond wavDiamond;
		wavDiamond.setInput(gP, gT, gK);
		
		PerformanceLap lap;
		int ed = wavDiamond.getDistance();
		lap.stop();
		
		printf("Wavefront Diamond distance=%d. Time: %0.5f seconds\n", ed, lap.lap());
	}
	
	// Test Diamond Wavefront 2 Cols
	if (doWFD2)
	{
		WavefrontDiamond2Cols wavDiamond;
		wavDiamond.setInput(gP, gT, gK);
		
		PerformanceLap lap;
		int ed = wavDiamond.getDistance();
		lap.stop();
		
		printf("Wavefront Diamond 2 cols distance=%d. Time: %0.5f seconds\n", ed, lap.lap());
	}
	
	// Test Dynamic Diamond Wavefront
	if (doWFDD)
	{
		WavefrontDynamicDiamond wavDynamicDiamond;
		wavDynamicDiamond.setInput(gP, gT);
		
		PerformanceLap lap;
		int ed = wavDynamicDiamond.getDistance();
		lap.stop();
		
		printf("Wavefront Dynamic Diamond distance=%d. Time: %0.5f seconds\n", ed, lap.lap());
	}
	
	// Test Dynamic Diamond Wavefront 2 Cols
	if (doWFDD2)
	{
		WavefrontDynamicDiamond2Cols wavDynamicDiamond;
		wavDynamicDiamond.setInput(gP, gT);
		
		PerformanceLap lap;
		int ed = wavDynamicDiamond.getDistance();
		lap.stop();
		
		printf("Wavefront Dynamic Diamond distance=%d. Time: %0.5f seconds\n", ed, lap.lap());
	}
	
	return 0;
}





