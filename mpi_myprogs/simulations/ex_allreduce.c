/******************************************************************************/
/*
		\file 		: 		ex_allreduce.c 										
		\author		:		Sreeram Sadasivam
		\copyright	:		Copyright 2015. All Rights Reserved.
		R\brief		:		Implementation of All reduce function using 
							simple send and receive primitives of MPI.
*/							
/******************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<string.h>

/** Definitions */

#define		ROOT 	0

/**
	Relative position function is used to reorder the ranks of nodes based on the root.

	Assume we have nodes 0,1,2,3,4,5,6,7. And we have the root node as 2
	After applying the function on this node list we have node list as
	2,3,4,5,6,7,0,1
	now 2 is the 0th rank, 3->1st rank,4->2nd rank and 7->5th rank and finally 1->7th rank.
*/


/** Implementation of relative position function of the buffer used. */
int relative_position(int parent_node,int root,int comm_size) {

	int relative_node;
	if(parent_node<root) {

		relative_node = parent_node + (comm_size-root);

	} else {

		relative_node = parent_node - root;

	}	
	return relative_node;
}


/**
	Absolute position function is used to find the actual rank of the node from relative rank
*/
/** Implementation of absolute position function of the buffer used.*/
int absolute_position(int relative_node,int root,int comm_size) {

	int abs_node;
	if(relative_node<(comm_size-root)) {
		abs_node = relative_node + root;
	} else {

		abs_node = relative_node - (comm_size-root);
	}
}

/** Implementation of the memory sum between memory locations. */
int mem_sum_int(int *dest,int *inputA,int *inputB,int buf_size) {

	int i;

	for(i=0;i<buf_size;i++) {

		*(dest+sizeof(int)*i) = *(inputA+sizeof(int)*i) + *(inputB+sizeof(int)*i);
	}
}

/** Implementation of reduce function used by OpenMPI */
int reduce(void *sendbuf,void *recvbuf,int buf_size,MPI_Datatype datatype,MPI_Op op,int root,MPI_Comm comm) {

	int comm_size,my_rank,i,r_node_pos;
	/**Communication processes size...*/
	MPI_Comm_size(comm,&comm_size);
	/**Communication process rank...*/
	MPI_Comm_rank(comm,&my_rank);

	r_node_pos = relative_position(my_rank,root,comm_size);

	for(i=1;i<=comm_size-1;i<<=1) {

		if(r_node_pos%(2*i)==0) {

			MPI_Recv(recvbuf,buf_size,datatype,absolute_position(r_node_pos+i,root,comm_size),0,comm,MPI_STATUS_IGNORE);
			if(op == MPI_SUM) {
				if(datatype == MPI_INT) {

					mem_sum_int((int*)sendbuf,(int*) sendbuf,(int*) recvbuf,buf_size);
					//printf("Actual: %d, Relative Node: %d, Relative Receive Node: %d\n",my_rank,r_node_pos,r_node_pos+i);		
					//printf("%d\n", *((int*) sendbuf));
				}
			}
		}
		else if((i==1)||(r_node_pos%i==0)) {

			//printf("Actual: %d, Relative Node: %d, Relative Send Node: %d\n",my_rank,r_node_pos,r_node_pos-i);	
			MPI_Send(sendbuf,buf_size,datatype,absolute_position(r_node_pos-i,root,comm_size),0,comm);
		}
		else {
			break;
		}
	}

//	if(my_rank==root) {

		memcpy(recvbuf,sendbuf,buf_size);
//	}
	return 0;
}
/** Implementation of Bcast function used by OpenMPI */
int bcast(void *buf,int buf_size,MPI_Datatype datatype,int root,MPI_Comm comm) {

	int comm_size,my_rank,i;

	/**Communication processes size...*/
	MPI_Comm_size(comm,&comm_size);
	/**Communication process rank...*/
	MPI_Comm_rank(comm,&my_rank);

	if(my_rank == root)
	{
		for(i=0;i<comm_size;i++) {
			if(my_rank != i) /*Checking node is itself or not... */{
				MPI_Send(buf,buf_size,datatype,i,1,comm);
			}
		}
	}	else {

		MPI_Recv(buf,buf_size,datatype,root,1,comm,MPI_STATUS_IGNORE);
	}
	return 0;
}

/** Implementation of All_Reduce function used by MPI */
int All_Reduce(void *data,void *res,int size,MPI_Datatype datatype,MPI_Op op,MPI_Comm comm) {

	int comm_size,my_rank;
	/**Communication processes size...*/
	MPI_Comm_size(comm,&comm_size);
	/**Communication process rank...*/
	MPI_Comm_rank(comm,&my_rank);

	reduce(data,res,size,datatype,op,ROOT,comm);
	//MPI_Barrier(comm);
	bcast(res,size,datatype,ROOT,comm);
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


	All_Reduce(&data,&sum,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
	printf("%d\n",sum );

	/** MPI Deallocates...*/
	MPI_Finalize();
	return 0;
}