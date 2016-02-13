#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


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
		
	/* Seed the random number generator with the current time */
	srand(MPI_Wtime()*1214123);
	iMyRandomNumber=(random()%100);

	printf("Rank %2.1i has the random number %i\n",iMyRank,iMyRandomNumber);
	
	/* broadcast */
	int myData=-1;
	if (iMyRank==0) printf("-------------------------------------\nbroadcast\n-------------------------------------\n");
	/* call the self implemented integer broadcast */
	myData=iMyRandomNumber;

	if (iMyRank==iInspectRank) printf("Rank %2.1i:\tmyData before the broadcast is %i\n",iInspectRank,myData);
	/* Call the self implemented broadcast */
	bcast_int(&myData,1,INSPECT_RANK,MPI_COMM_WORLD);
	
	printf("Rank %2.1i:\tmyData after the broadcast is %i\n",iMyRank,myData);

	MPI_Finalize();

	printf("Former rank %2.1i about to shut down\n",iMyRank);

	return 0;
}
