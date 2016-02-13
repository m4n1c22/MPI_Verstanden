/******************************************************************************/
/*
		\file 		: 		ex_allReduce.c 										
		\author		:		Sreeram Sadasivam
		\copyright	:		Copyright 2015. All Rights Reserved.
		\brief		:		Implementation of AllReduce function of MPI Library
							using simple send and receive primitives.
*/							
/******************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<string.h>


/** Implementation of the memory sum between memory locations. */
int mem_sum_int(int *dest,int *inputA,int *inputB,int buf_size) {

	int i;

	for(i=0;i<buf_size;i++) {

		*(dest+sizeof(int)*i) = *(inputA+sizeof(int)*i) + *(inputB+sizeof(int)*i);
	}
}

/** 
	Implementation of reduce function used by OpenMPI. The AllReduce function is implemented
	using the functionality of butterfly layout. which cross based reduction. The tree based
	layout makes use of only one node which is not efficient. 
*/

int AllReduce(void *sendbuf,void *recvbuf,int buf_size,MPI_Datatype datatype,MPI_Op op,MPI_Comm comm) {

	int comm_size,my_rank,i,r_node_pos;
	/**Communication processes size...*/
	MPI_Comm_size(comm,&comm_size);
	/**Communication process rank...*/
	MPI_Comm_rank(comm,&my_rank);

	for(i=1;i<=comm_size-1;i<<=1) {

		/** 
			Check if the round bit is 1 or not. If it is 1 then send it to the node which had given 0 in this
			round. If it is 0 then, send it to node which had 1 issued in  its node. But ensure that one node
			receives and other sends and not both sends.
		*/
		if((my_rank&i)==0) {

			MPI_Send(sendbuf,buf_size,datatype,my_rank+i,0,comm);
			MPI_Recv(recvbuf,buf_size,datatype,my_rank+i,0,comm,MPI_STATUS_IGNORE);
		}
		else {

			MPI_Recv(recvbuf,buf_size,datatype,my_rank-i,0,comm,MPI_STATUS_IGNORE);
			MPI_Send(sendbuf,buf_size,datatype,my_rank-i,0,comm);
		}

		if(op == MPI_SUM) {
			if(datatype == MPI_INT) {

				mem_sum_int((int*)sendbuf,(int*) sendbuf,(int*) recvbuf,buf_size);	
			}
		}
	}

	memcpy(recvbuf,sendbuf,buf_size);

	return 0;
}

int main(int argc,char *argv[]) {

	/** MPI Initialises...*/
	MPI_Init(&argc,&argv);

	int comm_size,my_rank,data,sum=0;

	/**Communication processes size...*/
	MPI_Comm_size(MPI_COMM_WORLD,&comm_size);
	/**Communication process rank...*/
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	data = my_rank;
	/** If rank is 0 means its the first process...*/
	AllReduce(&data,&sum,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
	printf("Rank: %d, Sum:%d\n",my_rank,sum );
	/** MPI Deallocates...*/
	MPI_Finalize();
	return 0;
}