
#ifndef FASTA_H
#define FASTA_H

class FastaInfo
{
public:
	char *seqName;
	long seqLen;
	char *seq;
};

int readSeq(FILE *fp, FastaInfo *fastaSeq);
/*read Fasta format Sequence*/
void freeFasta(FastaInfo * fastaSeq);
/*free Fasta sequence */


class FastaReader
{
public:
	static FastaInfo read(const char* file);
};

#endif /* End FASTA_H */