Examples

For short sequences 162,114 x 171,823 (distance = 92254).<br/>
CUDAlign execution time (GeForce GTX280) = 1.7 s 

<table>
  <tr><td>command-line</td><td>Intel(R) Core(TM) <br/>i7-5500U CPU<br/> @ 2.40GHz</td><td>Intel(R) Xeon(R)<br/> Silver 4210 CPU <br/>@ 2.20GHz</td></tr>
  <tr><td>./wavefront.exe -fP data/NC_000898.1.fasta -fT data/NC_007605.1.fasta -k -1 -DP2</td><td>113.49</td><td>95.37</td></tr> 
  <tr><td>./wavefront.exe -fP data/NC_000898.1.fasta -fT data/NC_007605.1.fasta -k -1 -WFO2</td><td>106.30</td><td>98.99</td></tr>
  <tr><td>./wavefront.exe -fP data/NC_000898.1.fasta -fT data/NC_007605.1.fasta -k -1 -WFD2</td><td>110.57</td><td>114.95</td></tr>
  <tr><td>./wavefront.exe -fP data/NC_000898.1.fasta -fT data/NC_007605.1.fasta -k -1 -WFDD2</td><td>89.75</td><td>84.37</td></tr>
</table>


For longer sequences 5,227,293 x 5,228,663 (distance = 2595).<br/>
CUDAlign execution time (GeForce GTX280) = 1,348 s

<table>
  <tr><td>command-line</td><td>Intel(R) Core(TM) <br/>i7-5500U CPU<br/> @ 2.40GHz</td><td>Intel(R) Xeon(R)<br/> Silver 4210 CPU <br/>@ 2.20GHz</td></tr>
  <tr><td>./wavefront.exe -fP data/AE016879.1.fasta -fT data/AE017225.1.fasta -k -1 -DP2 -v</td><td></td><td>101,202</td></tr>
  <tr><td>./wavefront.exe -fP data/AE016879.1.fasta -fT data/AE017225.1.fasta -k -1 -WFO2 -v</td><td></td><td>0.10</td></tr>
  <tr><td>./wavefront.exe -fP data/AE016879.1.fasta -fT data/AE017225.1.fasta -k -1 -WFD2 -v</td><td></td><td>0.12</td></tr>
  <tr><td>./wavefront.exe -fP data/AE016879.1.fasta -fT data/AE017225.1.fasta -k -1 -WFDD2 -v</td><td></td><td>0.12</td></tr>
</table>

Some files are reported in CUDAlign paper.

[1] Sandes, Edans Flavius O., and Alba Cristina MA de Melo. "CUDAlign: using GPU to accelerate the comparison of megabase genomic sequences." In Proceedings of the 15th ACM SIGPLAN symposium on Principles and practice of parallel programming, pp. 137-146. 2010.

[2] Marques, Thiago Costa, and Willian Picinato da Silva. "Comparação paralela de múltiplos pares de sequências biológicas em GPU e CPU com work stealing." (2017).

[3] Sadiq, Muhammad Umair, and Muhammad Murtaza Yousaf. "Distributed Algorithm for Parallel Edit Distance Computation." Computing and Informatics 39, no. 4 (2020): 757-779.

