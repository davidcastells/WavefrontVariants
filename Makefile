OPTIMIZATION=-O3

%.o: %.cpp
	g++ -g $(OPTIMIZATION) -fpermissive -I . -c $< -o $@
	
OBJECTS=wavefront.o LevDP.o LevDP2Cols.o WavefrontOriginal.o WavefrontOriginal2Cols.o WavefrontExtendPrecomputing.o WavefrontDiamond.o WavefrontDiamond2Cols.o WavefrontDynamicDiamond.o WavefrontDynamicDiamond2Cols.o fasta.o PerformanceLap.o 

all: wavefront

clean:
	rm -f *.o
	rm -f wavefront
	
wavefront: $(OBJECTS)
	g++ -g $(OPTIMIZATION) $(OBJECTS) -o wavefront