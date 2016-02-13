#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#define IDLETIME 0.1


double realtime (void) {
  struct timeval tv;
  gettimeofday(&tv, (struct timezone*)0);
  return ((double)tv.tv_sec + (double)tv.tv_usec / 1000000.0 );
}


double function(double x)
{
	double t=realtime();
	while(realtime()-t <= IDLETIME*x*x);
	return 4.0 / (1.0+x*x);
}

double masterIntegral(double (*function)(double x),double x_start,double x_end,int maxSteps) {

	int iMyRank,iNumProcs;
	MPI_Comm_rank(MPI_COMM_WORLD,&iMyRank);
	MPI_Comm_size(MPI_COMM_WORLD,&iNumProcs);

	double sum=0.0;
	double x[2],y[2];

	MPI_Status status;
	double stepSize=(x_end-x_start)/(double)maxSteps;
	int step;
	int nextRank=1,i;


	int *isWorking = (int*) malloc(iNumProcs*sizeof(int));
	/** Initialization of isWorking array*/
	for (i = 0; i < iNumProcs; ++i)
	{
		isWorking[i] = 0;
	}

	/* I am the master, distribute the work */
	for (step=0;step<maxSteps;step++)
	{
		/** Flag used for checking if all steps has been send or not.  */
		int stepIsSend = 0;

		/** 
			Loop to allot work to all processes with steps of pi but not all steps.
			This loop figures out idle processes and assigns work to them as well. 
		*/
		for (i = 1; i < iNumProcs; ++i)
		{
			/** Check if there are any processes which are idle. */
			if(isWorking[i]==0) {
				isWorking[i]=1;
				x[0]=x_start+stepSize*step;
				x[1]=x_start+stepSize*(step+1);
				MPI_Send(x,2,MPI_DOUBLE,i,0,MPI_COMM_WORLD);
				stepIsSend=1;
				break;
			}
		}
		/** Steps which are yet to be send to the workers. */
		if(!stepIsSend) {
			/** Wait for some process which will show with the result of its evaluation. */
			MPI_Recv(y,2,MPI_DOUBLE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
			sum+=stepSize*0.5*(y[0]+y[1]);
			x[0]=x_start+stepSize*step;
			x[1]=x_start+stepSize*(step+1);
			/** To make this process non-idle, assign some more work to this worker process. */
			MPI_Send(x,2,MPI_DOUBLE,status.MPI_SOURCE,0,MPI_COMM_WORLD);
		}
	}

	/** Receive results from all worker processes in the connection. */
	for (i = 1; i < iNumProcs; ++i)
	{
		if(isWorking[i]==1) {
			MPI_Recv(y,2,MPI_DOUBLE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
			sum+=stepSize*0.5*(y[0]+y[1]);
			isWorking[i]=0;
		}
	}
	/** Inform all the worker nodes to quit and stop waiting for more work. */
	for (nextRank=1;nextRank<iNumProcs;nextRank++)
		MPI_Send(&nextRank,0,MPI_INT,nextRank,2,MPI_COMM_WORLD);

	free(isWorking);
	return sum;
}

int workerIntegral() {

	double x[2],y[2];
	MPI_Status status;
	
	while(1) {
		/* I am a worker, wait for work */

		/* get the status for the next incoming message */
		MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD, &status);
		/* if the  tag is 2, that the signal to stop */
		if (status.MPI_TAG==2) break;
		/* receive the left and right side of the trapezoid and compute the corresponding function values */
		MPI_Recv(x,2,MPI_DOUBLE,0,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		y[0]=function(x[0]);
		y[1]=function(x[1]);
		/* send back the finished work */
		MPI_Send(y,2,MPI_DOUBLE,0,0,MPI_COMM_WORLD);	

	}
	return 0;
}



int main(int argc,char** argv)
{

	MPI_Init(&argc,&argv);
	double startTime,stopTime;
	int iMyRank;
	MPI_Comm_rank(MPI_COMM_WORLD,&iMyRank);
	
	
	if(iMyRank == 0)		
	{
		startTime=MPI_Wtime();
		double pi=masterIntegral(function,0.0,1.0,1000);
		stopTime=MPI_Wtime();
		printf("PI = %lf\t approximation = %lf\nComputation took %lf seconds\n",M_PI,pi,stopTime-startTime);
	}	else {

		workerIntegral();
	}
	
	MPI_Finalize();
	
}
