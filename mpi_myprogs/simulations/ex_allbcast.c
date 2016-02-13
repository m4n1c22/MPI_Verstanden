/******************************************************************************/
/*
		\file 		: 		ex_allbcast.c 										
		\author		:		Sreeram Sadasivam
		\copyright	:		Copyright 2015. All Rights Reserved.
		\brief		:		Implementation of allbcast function
							using simple send and receive primitives of MPI.
*/							
/******************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

/** Function to find modulus circle which can even handle for negative numbers. */

int modulo(int num,int moduli) {

	int modulus;

	modulus = num%moduli;
	if(modulus<0) {

		modulus = moduli + modulus;
	}
	return modulus;
}
/** Implementation of All Broadcast function using MPI Primitive functions. */
int All_Bcast(void *buf,int buf_size,MPI_Datatype datatype,MPI_Comm comm) {

	int comm_size,my_rank,i;

	/**Communication processes size...*/
	MPI_Comm_size(MPI_COMM_WORLD,&comm_size);
	/**Communication process rank...*/
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	if(my_rank%2==0) {

		for(i=0;i<comm_size;i++) {

			/** send(buf[(my_rank-i)%N],(my_rank+1)%N) */
			MPI_Send((buf+sizeof(int)*modulo((my_rank-i),comm_size)),1,datatype,modulo((my_rank+1),comm_size),0,comm);
			/** recv(buf[(my_rank-i-1)%N],(my_rank-1)%N) */
			MPI_Recv((buf+sizeof(int)*modulo((my_rank-i-1),comm_size)),1,datatype,modulo((my_rank-1),comm_size),0,comm,MPI_STATUS_IGNORE);
		}
	}	else {

		for(i=0;i<comm_size;i++) {

			/** recv(buf[(my_rank-i-1)%N],(my_rank-1)%N) */
			MPI_Recv((buf+sizeof(int)*modulo((my_rank-i-1),comm_size)),1,datatype,modulo((my_rank-1),comm_size),0,comm,MPI_STATUS_IGNORE);
			/** send(buf[(my_rank-i)%N],(my_rank+1)%N) */
			MPI_Send((buf+sizeof(int)*modulo((my_rank-i),comm_size)),1,datatype,modulo((my_rank+1),comm_size),0,comm);
		}

	}

	return 0;
}


/** Function which prints the array of the process. */
int printArrayWithProcessId(int array[],int size,int processID) {

	int i;
	printf("\nProcess %d Array: ",processID);
	for(i=0;i<size;i++) {

		printf("%d ", array[i]);
	}
	printf("\n");
	return 0;
}

int main(int argc,char *argv[]) {

	/** MPI Initialises...*/
	MPI_Init(&argc,&argv);

	int comm_size,my_rank;

	int *data;

	/**Communication processes size...*/
	MPI_Comm_size(MPI_COMM_WORLD,&comm_size);
	/**Communication process rank...*/
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	data = (int*) malloc(sizeof(int)*comm_size);
	/** If rank is 0 means its the first process...*/

	data[my_rank] = my_rank;
	All_Bcast(data,comm_size,MPI_INT,MPI_COMM_WORLD);

	printArrayWithProcessId(data,comm_size,my_rank);

	/** MPI Deallocates...*/
	MPI_Finalize();

	free(data);
	return 0;
}