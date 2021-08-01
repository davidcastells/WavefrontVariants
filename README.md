Examples

For short sequences (distance = 92254)

<table>
  <tr><td>command-line</td><td>Intel(R) Core(TM) <br/>i7-5500U CPU<br/> @ 2.40GHz</td><td>Intel(R) Xeon(R)<br/> Silver 4210 CPU <br/>@ 2.20GHz</td></tr>
  <tr><td>./wavefront.exe -fP data/NC_000898.1.fasta -fT data/NC_007605.1.fasta -k -1 -DP2</td><td>113.49</td><td>95.37</td></tr> 
  <tr><td>./wavefront.exe -fP data/NC_000898.1.fasta -fT data/NC_007605.1.fasta -k -1 -WFO2</td><td>106.30</td><td>98.99</td></tr>
  <tr><td>./wavefront.exe -fP data/NC_000898.1.fasta -fT data/NC_007605.1.fasta -k -1 -WFD2</td><td>110.57</td><td>114.95</td></tr>
  <tr><td>./wavefront.exe -fP data/NC_000898.1.fasta -fT data/NC_007605.1.fasta -k -1 -WFDD2</td><td>89.75</td><td>84.37</td></tr>
</table>


For longer sequences

./wavefront.exe -fP data/AE016879.1.fasta -fT data/AE017225.1.fasta -k -1 -DP2 -v<br/>
./wavefront.exe -fP data/AE016879.1.fasta -fT data/AE017225.1.fasta -k -1 -WFO2 -v<br/>

Some files are reported in CUDAlign paper.

Sandes, Edans Flavius O., and Alba Cristina MA de Melo. "CUDAlign: using GPU to accelerate the comparison of megabase genomic sequences." In Proceedings of the 15th ACM SIGPLAN symposium on Principles and practice of parallel programming, pp. 137-146. 2010.
