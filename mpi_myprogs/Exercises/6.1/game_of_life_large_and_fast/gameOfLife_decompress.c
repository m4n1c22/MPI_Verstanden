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
	
	N=atoi(*++argv);
	M=atoi(*++argv);
	InputFileName=*++argv;
	OutputFileName=*++argv;

	int humanReadable=0;
	if (argc>5 && strncmp(*++argv,"y",1)==0)
	{
		humanReadable=1;
	}
	else
	{
	}

	char * buffer;
	int bufferSize=N;
	
	buffer=(char*)malloc(sizeof(char)*bufferSize);

	
	

	
	CGOLF cgolf;
	CGOLF_Init(&cgolf,1024);
	CGOLF_in_open(&cgolf,InputFileName);
	/* Input end here, take time */
	
	FILE *of=fopen(OutputFileName,"w");
	if (of==NULL)
	{
		fprintf(stderr,"Error: Could not open %s for writing output\n",OutputFileName);
		exit(-1);
	}
	
	for (int m=0;m<M;m++)
	{
		CGOLF_in_read(&cgolf,N,buffer);
		if (humanReadable)
			for (int n=0;n<N;n++)
				if (buffer[n]) buffer[n]='*';
				else buffer[n]=' ';
		fwrite(buffer,N,1,of);
		if (humanReadable)
			fwrite("\n",1,1,of);
	}
	CGOLF_in_close(&cgolf);	
	fclose(of);	
	return 0;
}





