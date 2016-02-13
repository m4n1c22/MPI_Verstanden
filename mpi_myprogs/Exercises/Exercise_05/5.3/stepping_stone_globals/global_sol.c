#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

void bcast_int(void * data,int count,int root,MPI_Comm comm )
{
	
	int iMyRank,iNumProcs,i;

	MPI_Comm_size(comm,&iNumProcs);
	MPI_Comm_rank(comm,&iMyRank);

	if(iMyRank == root) {

		for (i = 0; i < iNumProcs; ++i)
		{
			if(i != root) {

				MPI_Send(data,count,MPI_INT,i,0,comm);
			}
		}		
		
	} else {

		MPI_Recv(data,count,MPI_INT,root,0,comm,MPI_STATUS_IGNORE);
	}
}

void reduce_plus_double(void *sendbuf, 		void *recvbuf, 		int cnt, 		int root, 		MPI_Comm comm )
{
	int iMyRank,iNumProcs,i,j;	

	MPI_Comm_size(comm,&iNumProcs);
	MPI_Comm_rank(comm,&iMyRank);


	if(iMyRank == root) {

		/** 
			In-effcient method but will suffice. Here first the data is send to all worker nodes to 
		*/		
		
		printf("\n BUFFER: %lf",*((double*) sendbuf));

		for (i = 0; i < iNumProcs; ++i)
		{
			if(iMyRank != i) {

				MPI_Recv(recvbuf,cnt,MPI_DOUBLE,MPI_ANY_SOURCE,0,comm,MPI_STATUS_IGNORE);	
				for(j=0;j<cnt;j++) {

					*((double*) sendbuf+j) = (double) *((double*) sendbuf+j) + (double) *((double*) recvbuf+j);
				}
			}		
		}
		printf("\n BUFFER: %lf",*((double*) sendbuf));
		memcpy(recvbuf,sendbuf,cnt*sizeof(double));		

	}	else {

			MPI_Send(sendbuf,cnt,MPI_DOUBLE,root,0,comm);	
	}

}

void barrier (MPI_Comm comm)
{
	int iMyRank,iNumProcs,i,j;

	double *tmp;
	MPI_Comm_size(comm,&iNumProcs);
	MPI_Comm_rank(comm,&iMyRank);

	for(i=0;i<iNumProcs;i++) {

		if(i==iMyRank) {

			for(j=0;j<iNumProcs;j++) {
				if(j!=iMyRank) {
					MPI_Send(NULL,0,MPI_CHAR,j,i,comm);
				}				
			}
		}
		else {
				MPI_Recv(tmp,0,MPI_CHAR,i,i,comm,MPI_STATUS_IGNORE);			
		}
	}
}

#define INSPECT_RANK 0

int main(int argc,char ** argv)
{
	
	MPI_Init(&argc,&argv);
	int iMyRank,iNumProcs,iMyRandomNumber,iInspectRank=INSPECT_RANK;
	MPI_Comm_size(MPI_COMM_WORLD,&iNumProcs);
	MPI_Comm_rank(MPI_COMM_WORLD,&iMyRank);
	/* Declare and Init buffers */
	int i;
		
	/* Seed the random number generator with the current time */
	srand(MPI_Wtime()*1214123);
	iMyRandomNumber=(random()%100);

	if (argc > 1)
	{
		iInspectRank= atoi(argv[1]);
		if (iInspectRank<0) MPI_Abort(MPI_COMM_WORLD,1);
	}	
	printf("Rank %2.1i has the random number %i\n",iMyRank,iMyRandomNumber);
	
	/* broadcast */
	int myData=-1;
	barrier(MPI_COMM_WORLD);
	if (iMyRank==0) printf("-------------------------------------\nbroadcast\n-------------------------------------\n");
	/* call the self implemented integer broadcast */
	myData=iMyRandomNumber;

	if (iMyRank==iInspectRank) printf("Rank %2.1i:\tmyData before the broadcast is %i\n",iInspectRank,myData);
	/* Call the self implemented broadcast */
	bcast_int(&myData,1,0,MPI_COMM_WORLD);
	printf("Rank %2.1i:\tmyData after the broadcast is %i\n",iMyRank,myData);

	
	
	/* reduce */
	barrier(MPI_COMM_WORLD);
	if (iMyRank==0) printf("-------------------------------------\nreduce\n-------------------------------------\n");
	double myDoubleData=iMyRandomNumber;
	double myDoubleResult=FP_NAN;
	if (iMyRank==iInspectRank) printf("Rank %2.1i:\tmyData before the reduce operation %lf\n",iInspectRank,myDoubleData);
	/* Call the self implemented reduce */
	reduce_plus_double(&myDoubleData,&myDoubleResult,1,0,MPI_COMM_WORLD);
	if (iMyRank==iInspectRank) printf("Rank %2.1i:\tmyData after the reduce operation is %lf\n",iInspectRank,myDoubleResult);
		
	barrier(MPI_COMM_WORLD);
	MPI_Finalize();

	printf("Former rank %2.1i about to shut down\n",iMyRank);

	return 0;
}
