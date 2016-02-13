#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"
#include "CGOLF.h"

typedef enum {
	BOUNDRY_LOWERBOUND=0,
	BOUNDRY_ALIVE=1,
	BOUNDRY_DEAD=2,
	BOUNDRY_PERIODIC=3,
	BOUNDRY_UPPERBOUND} boundryType;

#ifndef BOUNDRY_TYPE
	const boundryType BOUNDRY=BOUNDRY_DEAD;
#else
	const boundryType BOUNDRY=BOUNDRY_TYPE;
#endif

int main(int argc,char** argv)
{
	/* YOU DO NOT NEED TO MODIFY: End */
	int N,M,Iterations,NumberOfOutputIterations;
	int OutputCounter;
	char *InputFileName;
	char *OutputFileName;
	double SetupStart,SetupEnd,ComputeStart,ComputeEnd;
	/* YOU DO NOT NEED TO MODIFY: Begin */
	
	/* YOU DO NOT NEED TO MODIFY: Begin */
	/* Manage Command Line Interface */
	int i;
	/* Process Input from command-line arguments */
	if (argc< 5)
		fprintf(stderr,"gameOfLife_compress.exe N M InputFile OutputFile [humanReadableSrc]\n\tN,M: 1 ... 2^20\n\tInputfile: game filed; NxM elements, 0=dead, !0=alive, or if human readable \"*\" or 1 alive, dead otherwise\n\t Outfile: compressed game of life file\n\t [y]: input is in human readable format, including linebreaks\n");

	N=atoi(*++argv);
	M=atoi(*++argv);
	InputFileName=*++argv;
	OutputFileName=*++argv;

	int humanReadable=0;
    if (argc>5 && strncmp(*++argv,"y",1)==0)
    {
        humanReadable=1;
    }

	char * buffer;
	int bufferSize=N;
	
	buffer=(char*)malloc(sizeof(char)*bufferSize);
	
	CGOLF cgolf;
	CGOLF_Init(&cgolf,1024);
	CGOLF_out_open(&cgolf,OutputFileName);
	/* Input end here, take time */
	
	FILE *of=fopen(InputFileName,"r");
	
	for (int m=0;m<M;m++)
	{
		fread(buffer,N,1,of);

		if (humanReadable)
			for (int n=0;n<N;n++)
				if (buffer[n]=='*' || buffer[n]=='1') buffer[n]=1;
				else buffer[n]=0;
		CGOLF_out_write(&cgolf,N,buffer);
		if (humanReadable)
		fread(buffer,1,1,of);
	}
	CGOLF_out_close(&cgolf);	
	fclose(of);	
	return 0;
}
