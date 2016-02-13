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
    /* YOU DO NOT NEED TO MODIFY: Begin */
	int N,M,Iterations,NumberOfOutputIterations;
	int OutputCounter;
	char *InputFileName;
	int* OutputIterations=NULL;
	double SetupStart,SetupEnd,ComputeStart,ComputeEnd;
    /* YOU DO NOT NEED TO MODIFY: End */
	
	/* YOU DO NOT NEED TO MODIFY: Begin */
	/* Manage Command Line Interface */
	int i;
	/* Process Input from command-line arguments */
	if (argc< 5)
		fprintf(stderr,"%s N M InputFile TotalIt NumOut It1 It2 ... It(NumOut)\n\tN,M: 1 ... 2^20\n\tInputfile: ZLIB compressed game filed; NxM elements, 0=dead, !0=alive\n\t TotalIt: Number of total iterations\n\t NumOut: Number of output-times\n\t It1 ... It(NumOut): NumOut-iteration times for output\n",argv[0]);

	N=atoi(*++argv);
	M=atoi(*++argv);
	InputFileName=*++argv;
	Iterations=atoi(*++argv);
	NumberOfOutputIterations=atoi(*++argv);
	fprintf(stderr,"Reading %i output times\n",NumberOfOutputIterations);
	if (NumberOfOutputIterations<0 || (OutputIterations=(int*)malloc(sizeof(int)*(NumberOfOutputIterations+1)))==NULL)
	{
		return -1;
	}
	else
	{
		OutputCounter=0;
		while(OutputCounter<NumberOfOutputIterations)
		{

			OutputIterations[OutputCounter++]=atoi(*++argv);
		}
	}
	OutputIterations[OutputCounter++]=Iterations;
	
	char * OutputFileNameBase=InputFileName;
	int ofnbLen=strnlen(InputFileName,1000);
	OutputFileNameBase=(char*)malloc(sizeof(char)*(ofnbLen+1));
	strncpy(OutputFileNameBase,InputFileName,ofnbLen);
	
	if (ofnbLen>5 &&InputFileName[ofnbLen-1]=='f' && InputFileName[ofnbLen-2]=='l' && InputFileName[ofnbLen-3]=='o' && InputFileName[ofnbLen-4]=='g' && InputFileName[ofnbLen-5]=='c' && InputFileName[ofnbLen-6]=='.' )
	{
		// it ends with .golf 
		OutputFileNameBase[ofnbLen-6]=0;

	}

	/* YOU DO NOT NEED TO MODIFY: End */

	


	/* Implement the game of life here */
	unsigned long gameFieldSize=N*M;
	unsigned char * gameFieldSrc;
	unsigned char * gameFieldDst;
	unsigned char * gameFieldTmp;
	gameFieldSrc=(unsigned char*)malloc(sizeof(unsigned char)*gameFieldSize);
	gameFieldDst=(unsigned char*)malloc(sizeof(unsigned char)*gameFieldSize);
	if (gameFieldSrc==NULL || gameFieldDst==NULL)
	{
		fprintf(stderr,"Malloc for game field failed: tried to allocated %lu bytes of memory\n",gameFieldSize);
	}
	
	/* Implement Input & Input distribution here */
	omp_get_wtime ();
	CGOLF cgolf;
	CGOLF_Init(&cgolf,1024);
	CGOLF_in_open(&cgolf,InputFileName);
	CGOLF_in_read(&cgolf,gameFieldSize,gameFieldSrc);
	CGOLF_in_close(&cgolf);	
	/* Input end here, take time */
	
	for (int i=0;i<N;i++){
		for (int j=0;j<M;j++)
		{
			if (gameFieldSrc[i*M+j]) printf("o");
			else printf(" ");
		}
		printf("\n");
	}

	unsigned long iteration;
	int nextOutput=0;
	for (iteration=0;iteration<Iterations;iteration++)
	{
		for (int i=0;i<N;i++){
			for (int j=0;j<M;j++)
			{
				int n,m;
				int livingNeighbours=0;
				int visitedNeighbours=0;
				for (int k=-1;k<2;k++)
{
					for (int l=-1;l<2;l++)
					{
						n=i+k;
						m=j+l;
						// skip for the current cell
						if (n==i && m==j) continue;
				
						if (n<0 || n>=N ||
						    m<0 || m>=M) 
						{
							// we are a border-cell
							if (BOUNDRY==BOUNDRY_ALIVE) 
							{
								livingNeighbours++;
								continue;
							}
							else if (BOUNDRY==BOUNDRY_DEAD)
								continue;	
							else
							{
								// Periodic case: adapt n and m
								if (n<0 || n>=N) n=(n+N)%N;
								if (m<0 || m>=M) m=(m+M)%M;
								
							}
						}
						visitedNeighbours++;
						if (gameFieldSrc[n*M+m]) livingNeighbours++;
					}
				}
//			printf("%i",livingNeighbours);
//			printf("%i",'0'+visitedNeighbours);
			
			switch (livingNeighbours)
				{
					case 2:
						gameFieldDst[i*M+j]=gameFieldSrc[i*M+j];
						break;
					case 3:
						gameFieldDst[i*M+j]=1;
						break;
					default:
						gameFieldDst[i*M+j]=0;
								
				}
	if (gameFieldDst[i*M+j])	printf("%c",'0');	else printf(" ");

							
			}
		printf("\n");
		}
		printf("\n\n");
		// Is the current time step in the output list?
		if (nextOutput < NumberOfOutputIterations && OutputIterations[nextOutput]==iteration)
		{
				char *outputName;
				outputName=(char*)malloc(strnlen(OutputFileNameBase,512)+10);
				sprintf(outputName,"%s.%i.cgolf",OutputFileNameBase,iteration);
	
				CGOLF_out_open(&cgolf,outputName);
				CGOLF_out_write(&cgolf,gameFieldSize,gameFieldDst);
				CGOLF_out_close(&cgolf);
				nextOutput++;
		}
		

		// swap src and dst
		gameFieldTmp=gameFieldDst;
		gameFieldDst=gameFieldSrc;
		gameFieldSrc=gameFieldTmp;
	}

/* Final result: has to be done ALLWAYS! */
	{
		char *outputName;
		outputName=(char*)malloc(strnlen(OutputFileNameBase,512)+10);
		sprintf(outputName,"%s.end.cgolf",OutputFileNameBase);
		CGOLF_out_open(&cgolf,outputName);
		CGOLF_out_write(&cgolf,gameFieldSize,gameFieldSrc);
		CGOLF_out_close(&cgolf);
	}

	return 0;
}





