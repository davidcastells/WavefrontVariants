# Do not modify, this makefile was automatically created by the configure script
OPTIMIZATION=-O2
CFLAGS=-g
OPENCL=-l OpenCL

%.o: %.cpp
	nvcc  $(OPTIMIZATION) $(CFLAGS) -I . -c $< -o $@

%.o: %.cu
	nvcc  $(OPTIMIZATION) $(CFLAGS) -I . -c $< -o $@

OBJECTS=wavefront.o LevDP.o LevDP2Cols.o WavefrontOriginal.o  \
	WavefrontOriginal2Cols.o WavefrontExtendPrecomputing.o  \
	WavefrontDiamond.o WavefrontDiamond2Cols.o WavefrontDynamicDiamond.o \
	WavefrontDynamicDiamond2Cols.o fasta.o utils.o  PerformanceLap.o \
	OCLGPUWavefrontOriginal2Cols.o OCLFPGAWavefrontOriginal2Cols.o OCLUtils.o \
	CUDAWavefrontOriginal2Cols.o 

all: wavefront

clean:
	rm -f *.o
	rm -f wavefront

wavefront: $(OBJECTS)
	nvcc  $(OPTIMIZATION) $(CFLAGS) $(OBJECTS) $(OPENCL) -o wavefront
