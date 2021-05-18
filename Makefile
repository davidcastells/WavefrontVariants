all:
	g++ -g -O2 fasta.cpp wavefront.cpp WavefrontOriginal.cpp WavefrontExtendPrecomputing.cpp WavefrontDiamond.cpp WavefrontDynamicDiamond.cpp LevDP.cpp PerformanceLap.cpp -o wavefront.exe