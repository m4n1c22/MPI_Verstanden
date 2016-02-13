/******************************************************************************/
/*
		\file 		: 		ex_barrier.c 										
		\author		:		Sreeram Sadasivam
		\copyright	:		Copyright 2015. All Rights Reserved.
		\brief		:		Implementation of barrier function of MPI Library
							using simple send and receive primitives.
*/							
/******************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

int barrier(MPI_Comm comm) {

	int comm_size,my_rank,i,j;

	/**Communication processes size...*/
	MPI_Comm_size(comm,&comm_size);
	/**Communication process rank...*/
	MPI_Comm_rank(comm,&my_rank);

	for(i=0;i<comm_size;i++) {
		/***/
		if(my_rank == i) {
			for (j = 0; j < comm_size; j++)
			{
				/**Condition check to see if I am sending to myself...*/
				if(j!=my_rank) {
					MPI_Send(NULL,0,MPI_INT,j,i,comm);
				}
			}				
		}
		else {

			MPI_Recv(NULL,0,MPI_INT,i,i,comm,MPI_STATUS_IGNORE);
		}
	}
	return 0;
}

int main(int argc,char *argv[]) {

	/** MPI Initialises...*/
	MPI_Init(&argc,&argv);

	int comm_size,my_rank,data;

	/**Communication processes size...*/
	MPI_Comm_size(MPI_COMM_WORLD,&comm_size);
	/**Communication process rank...*/
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	barrier(MPI_COMM_WORLD);

	printf("%d\n",data );
	/** MPI Deallocates...*/
	MPI_Finalize();
	return 0;
}