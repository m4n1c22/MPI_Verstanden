/*****************************************************************************/
/**			\file 		:		hello_world.c
			\author		: 		Sreeram Sadasivam
			\Copyright	: 		Copyright 2015. All Rights reserved.
			\brief		:		This a hello world program using the MPI
								Library.		
*/
/*****************************************************************************/
#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int solution_for_partA() {	

	int iMyRank, iNumProcs;
  
 	int data;

	MPI_Comm_rank(MPI_COMM_WORLD,&iMyRank);
 	MPI_Comm_size(MPI_COMM_WORLD,&iNumProcs);

  	// TODO: have only the first process execute the following code
  	if (iMyRank == 0) {

	  	//scanf("%d",&data); 	
	  	data = rand();
      	printf("Sending Ping (%d)\n",data);
      	// TODO: send data
      	MPI_Send(&data,1,MPI_INT,1,0,MPI_COMM_WORLD);
      	MPI_Recv(&data,1,MPI_INT,1,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      	// TODO: receive data
	  	printf("Received Pong (%d)\n",data);
 	} 	else {  

      MPI_Recv(&data,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      printf("Received Ping (%d)\n",data);
      data = -1*data;
      printf("Sending Pong (%d)\n",data);
      MPI_Send(&data,1,MPI_INT,0,1,MPI_COMM_WORLD);
    }

}

int solution_for_partB() {	

	int iMyRank, iNumProcs,i;
  
 	int *Array;

 	unsigned char num_elements = rand()%15; 

	MPI_Comm_rank(MPI_COMM_WORLD,&iMyRank);
 	MPI_Comm_size(MPI_COMM_WORLD,&iNumProcs);

 	printf("num_elements %d\n", num_elements);



  	// TODO: have only the first process execute the following code
  	if (iMyRank == 0) {

	  	//scanf("%d",&data); 	
	  	
  		/** Initialisation of array using random values. */
  		Array = (int*) malloc(num_elements*sizeof(int));
 		
  		printf("\nProcess 0 Sending Ping:\n");
  		for(i=0;i<num_elements;i++) {

  			Array[i] = rand()%100;
  			printf("\nValue %d: %d",(i+1),Array[i]);
  		}

      	
      	// TODO: send data
      	MPI_Send(&num_elements,1,MPI_INT,1,0,MPI_COMM_WORLD);
      	MPI_Send(Array,num_elements,MPI_INT,1,1,MPI_COMM_WORLD);
      	
      	MPI_Recv(Array,num_elements,MPI_INT,1,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      	// TODO: receive data
 		printf("\nProcess 0 Received Pong:\n");
  		for(i=0;i<num_elements;i++) {

  			printf("\nValue %d: %d",(i+1),Array[i]);
  		}

	  	free(Array);

 	} 	else {  

 		MPI_Recv(&num_elements,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);	
 		Array = (int*) malloc(num_elements*sizeof(int));
      	MPI_Recv(Array,num_elements,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
 		printf("\nProcess 1 Received Ping:\n");
  		for(i=0;i<num_elements;i++) {

  			printf("\nValue %d: %d",(i+1),Array[i]);

  		}

  		printf("\nProcess 1 Sending Pong:\n");
  		for(i=0;i<num_elements;i++) {

  			Array[i] = -1*Array[i];
  			printf("\nValue %d: %d",(i+1),Array[i]);
  		}
      	MPI_Send(Array,num_elements,MPI_INT,0,2,MPI_COMM_WORLD);
      	free(Array);
    }

    
    //free(recvArray);
}


int solution_for_partC() {

	int iMyRank, iNumProcs, data,i;

	/** Initialization of process rank and size of communicator. */
	MPI_Comm_rank(MPI_COMM_WORLD,&iMyRank);
	MPI_Comm_size(MPI_COMM_WORLD,&iNumProcs);

	if(iMyRank == 0) {

		for(i=0;i<iNumProcs;i++) {

			if(i!=iMyRank) {
			
				data = rand();
				printf("Process %d Sending Ping (%d) to %d\n",iMyRank,data,i);
				MPI_Send(&data,1,MPI_INT,i,0,MPI_COMM_WORLD);
			}
		}
		for(i=0;i<iNumProcs;i++) {

			if(i!=iMyRank) {
				MPI_Recv(&data,1,MPI_INT,i,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				printf("Process %d Received Pong (%d) from %d\n",iMyRank,data,i);
			}
		}
	}	else {

		MPI_Recv(&data,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    	printf("Process %d Received Ping (%d)from 0 \n",iMyRank,data);
      	data = -1*data;
      	printf("Process %d Sending Pong (%d) to 0\n",iMyRank,data);		
      	MPI_Send(&data,1,MPI_INT,0,1,MPI_COMM_WORLD);
	}

}

int solution_for_partD() {	

	int iMyRank, iNumProcs,i;
  
 	int *Array;

	int num_elements = rand()%15; 
 	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD,&iMyRank);
 	MPI_Comm_size(MPI_COMM_WORLD,&iNumProcs);

 	printf("num_elements %d\n", num_elements);



  	// TODO: have only the first process execute the following code
  	if (iMyRank == 0) {

	  	//scanf("%d",&data); 	
	  	
  		/** Initialisation of array using random values. */
  		Array = (int*) malloc(num_elements*sizeof(int));
 		
  		printf("\nProcess 0 Sending Ping:\n");
  		for(i=0;i<num_elements;i++) {

  			Array[i] = rand()%100;
  			printf("\nValue %d: %d",(i+1),Array[i]);
  		}

      	
      	// TODO: send data
      	MPI_Send(Array,num_elements,MPI_INT,1,0,MPI_COMM_WORLD);
      	
      	MPI_Recv(Array,num_elements,MPI_INT,1,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      	// TODO: receive data
 		printf("\nProcess 0 Received Pong:\n");
  		for(i=0;i<num_elements;i++) {

  			printf("\nValue %d: %d",(i+1),Array[i]);
  		}

	  	free(Array);

 	} 	else {  

 		
 		MPI_Probe(0,0,MPI_COMM_WORLD,&status);
 		MPI_Get_count(&status,MPI_INT,&num_elements);

 		Array = (int*) malloc(num_elements*sizeof(int));
      	
      	MPI_Recv(Array,num_elements,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
 		printf("\nProcess 1 Received Ping:\n");
  		for(i=0;i<num_elements;i++) {

  			printf("\nValue %d: %d",(i+1),Array[i]);

  		}

  		printf("\nProcess 1 Sending Pong:\n");
  		for(i=0;i<num_elements;i++) {

  			Array[i] = -1*Array[i];
  			printf("\nValue %d: %d",(i+1),Array[i]);
  		}
      	MPI_Send(Array,num_elements,MPI_INT,0,1,MPI_COMM_WORLD);
      	free(Array);
    }
}


int main(int argc, char **argv)
{
  // TODO: initialize MPI, find out MPI size and rank
  MPI_Init(&argc,&argv);

  solution_for_partD();
  // TODO: finish MPI
  MPI_Finalize();

  return 0;
}
