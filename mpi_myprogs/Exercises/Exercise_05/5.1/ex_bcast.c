#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

int bcast(void *buf,int buf_size,MPI_Datatype datatype,int root,MPI_Comm comm) {

	int comm_size,my_rank,i;

	/**Communication processes size...*/
	MPI_Comm_size(MPI_COMM_WORLD,&comm_size);
	/**Communication process rank...*/
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	if(my_rank == root)
	{
		for(i=0;i<comm_size;i++) {
			if(my_rank != i) {
				MPI_Send(&buf,buf_size,datatype,i,0,MPI_COMM_WORLD);
			}
		}
	}	else {

		MPI_Recv(&buf,buf_size,datatype,root,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}

	return 0;
}

int main(int argc,char *argv[]) {

	/** MPI Initialises...*/
	MPI_Init(&argc,&argv);

	int comm_size,my_rank,data,*X;

	/**Communication processes size...*/
	MPI_Comm_size(MPI_COMM_WORLD,&comm_size);
	/**Communication process rank...*/
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	X = (int*) malloc(sizeof(int)*comm_size);
	/** If rank is 0 means its the first process...*/
	if(my_rank == 0) {

		data = 0;
		bcast(&data,1,MPI_INT,0,MPI_COMM_WORLD);
	}	else {

		bcast(&data,1,MPI_INT,0,MPI_COMM_WORLD);
		printf("%d\n",data );
	}
	/** MPI Deallocates...*/
	free(X);
	MPI_Finalize();
	return 0;
}