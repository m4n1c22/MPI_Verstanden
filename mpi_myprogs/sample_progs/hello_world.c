/**
	\file		hello_world.c
		 		This is a hello_world program for depicting the running
		 		of an MPI program.
	\author		Sreeram Sadasivam
	\copyright	Copyright 2015 by Sreeram Sadasivam. All rights reserved.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mpi.h>

/** \def MAXSTRING
	This is a macro which defines the maximum size to the communicating 
	message. */
#define MAXSTRING 100

/**************************************************************************/
/**		\fn main()
		\param 	argc - argument count
		\param 	argv - arguments
		\return EXIT SUCCESS
		\brief	MAIN FUNCTION.
*/		
/**************************************************************************/

int main(int argc, char *argv[]) {

	/** greetinss message. */
	char 	greetings[100];
	/** communicator size */
	int 	comm_size;
	/**	current process rank */
	int 	my_rank;

	/** function call to allocate the communicator, message buffers. */
	MPI_Init(&argc,&argv);

	/** function call to get the communicator size. */
	MPI_Comm_size(MPI_COMM_WORLD,&comm_size);

	/** function call to get the rank of the process in the communicator*/
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	if(my_rank != 0) {

		sprintf(greetings,"greetings from process %d of %d!",my_rank,comm_size);

		/** Function call to send the greetings via communicator. */
		MPI_Send(greetings,strlen(greetings)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
	}
	else {

		printf("Greetings from process %d of %d!\n", my_rank,comm_size);
		for(int i=1;i<comm_size;i++) {

			/** Function call to receive the greetings via communicator. */
			MPI_Recv(greetings,MAXSTRING,MPI_CHAR,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			printf("%s\n",greetings);
		}
	}
	/** function call to de-allocate the communicator, message buffers. */
	MPI_Finalize();

	return 0;
}