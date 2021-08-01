Examples

For short sequences (distance = 92254)
./wavefront.exe -fP data/NC_000898.1.fasta -fT data/NC_007605.1.fasta -k -1 -DP2 -v (112 seconds on laptop)
./wavefront.exe -fP data/NC_000898.1.fasta -fT data/NC_007605.1.fasta -k -1 -WFO2 -v (108 seconds on laptop)
./wavefront.exe -fP data/NC_000898.1.fasta -fT data/NC_007605.1.fasta -k -1 -WFD2 -v (112 seconds on laptop)
./wavefront.exe -fP data/NC_000898.1.fasta -fT data/NC_007605.1.fasta -k -1 -WFDD2 -v (xxx seconds on laptop)


For longer sequences

./wavefront.exe -fP data/AE016879.1.fasta -fT data/AE017225.1.fasta -k -1 -DP2 -v
./wavefront.exe -fP data/AE016879.1.fasta -fT data/AE017225.1.fasta -k -1 -WFO2 -v

Some files are reported in CUDAlign paper.

Sandes, Edans Flavius O., and Alba Cristina MA de Melo. "CUDAlign: using GPU to accelerate the comparison of megabase genomic sequences." In Proceedings of the 15th ACM SIGPLAN symposium on Principles and practice of parallel programming, pp. 137-146. 2010.