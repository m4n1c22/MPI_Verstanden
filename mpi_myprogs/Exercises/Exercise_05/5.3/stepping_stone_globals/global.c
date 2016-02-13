#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


void bcast_int(void * data,int count,int root,MPI_Comm comm )
{
	/* TODO: Implement the broadcast using Send & Receive here */
}
void reduce_plus_double(void *sendbuf, 		void *recvbuf, 		int cnt, 		int root, 		MPI_Comm comm )
{
	/* TODO: Implement the reduce operation using Send & Receive here */
}

void barrier (MPI_Comm comm)
{
	/* TODO: Implement the barrier operation using only Send & Receive */
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
	int *mySendBuffer=(int*)malloc(sizeof(int)*iNumProcs);
	int *myRecvBuffer=(int*)malloc(sizeof(int)*iNumProcs);
		
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
	if (iMyRank==iInspectRank) printf("Rank %2.1i:\tmyData after the broadcast is %i\n",iInspectRank,myData);
	/* TODO: Use the MPI_Bcast here */
	if (iMyRank==iInspectRank) printf("Rank %2.1i:\tmyData after the MPI broadcast is %i\n",iInspectRank,myData);

	
	
	/* reduce */
	barrier(MPI_COMM_WORLD);
	if (iMyRank==0) printf("-------------------------------------\nreduce\n-------------------------------------\n");
	double myDoubleData=iMyRandomNumber;
	double myDoubleResult=FP_NAN;
	if (iMyRank==iInspectRank) printf("Rank %2.1i:\tmyData before the reduce operation %lf\n",iInspectRank,myDoubleResult);
	/* Call the self implemented reduce */
	reduce_plus_double(&myDoubleData,&myDoubleResult,1,0,MPI_COMM_WORLD);
	if (iMyRank==iInspectRank) printf("Rank %2.1i:\tmyData after the reduce operation is %i\n",iInspectRank,myData);
	/* TODO: Use the MPI Reduce operation here i*/
	if (iMyRank==iInspectRank) printf("Rank %2.1i:\tmyData after the MPI-reduce operation is %i\n",iInspectRank,myData);
	
	barrier(MPI_COMM_WORLD);
	MPI_Finalize();

	printf("Former rank %2.1i about to shut down\n",iMyRank);
}
