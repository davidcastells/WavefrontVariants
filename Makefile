OPTIMIZATION=-O3

OS=`uname -o
CFLAGS=-g -gdwarf-4 
#-fpic -fopenmp 
#CFLAGS=-lopencl
OPENCL=-l OpenCL

%.o: %.cpp
	g++  $(OPTIMIZATION) $(CFLAGS) -fpermissive -I . -c $< -o $@
	
OBJECTS=wavefront.o LevDP.o LevDP2Cols.o WavefrontOriginal.o WavefrontOriginal2Cols.o WavefrontExtendPrecomputing.o WavefrontDiamond.o WavefrontDiamond2Cols.o WavefrontDynamicDiamond.o WavefrontDynamicDiamond2Cols.o fasta.o PerformanceLap.o OCLUtils.o OCLGPUWavefrontOriginal2Cols.o
#/usr/lib/libOpenCL.dll.a

all: wavefront

clean:
	rm -f *.o
	rm -f wavefront
	
wavefront: $(OBJECTS)
	g++  $(OPTIMIZATION) $(CFLAGS) $(OBJECTS) $(OPENCL) -o wavefront
