# Do not modify, this makefile was automatically created by the configure script
OPTIMIZATION=-O2
CFLAGS=-g -gdwarf-4
OPENCL=-l OpenCL

%.o: %.cpp
	g++  $(OPTIMIZATION) $(CFLAGS) -fpermissive -I . -c $< -o $@

OBJECTS=wavefront.o LevDP.o LevDP2Cols.o WavefrontOriginal.o  \
	WavefrontOriginal2Cols.o WavefrontExtendPrecomputing.o  \
	WavefrontDiamond.o WavefrontDiamond2Cols.o WavefrontDynamicDiamond.o \
	WavefrontDynamicDiamond2Cols.o fasta.o PerformanceLap.o \
	OCLGPUWavefrontOriginal2Cols.o OCLFPGAWavefrontOriginal2Cols.o OCLGPUWavefrontDynamicDiamond2Cols.o OCLUtils.o 

all: wavefront

clean:
	rm -f *.o
	rm -f wavefront

wavefront: $(OBJECTS)
	g++  $(OPTIMIZATION) $(CFLAGS) $(OBJECTS) $(OPENCL) -o wavefront
