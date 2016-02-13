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

	double sum=0.0,partSum=0.0;

	MPI_Status status;
	double stepSize=(x_end-x_start)/(double)maxSteps;
	int step=1;
	int nextRank=1,i;
	int openMsgCount=0;

	/** Maximum number of strides used by the processes for processing the integral. */
	int stride = maxSteps/(iNumProcs-1);

	/* I am the master, distribute the work */
	while(step<=maxSteps)
	{
		/** 
			Loop to allot work to all processes with steps of pi but not all steps.
			This loop figures out idle processes and assigns work to them as well. 
		*/
		for (i = 1; (i < iNumProcs)&&(step<=maxSteps); ++i)
		{

			/** Check if there are any processes which are idle. */
			//double startX = (x_start+((double)step-0.5)*(x_end-x_start))/maxSteps;
			double startX = (x_start+(x_end-x_start)*(double)((i-1)*stride+0.5))/maxSteps;
			double delta=(x_end-x_start)/maxSteps;

			MPI_Send(&startX,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD);
			MPI_Send(&delta,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD);
			MPI_Send(&stride,1,MPI_INT,i,0,MPI_COMM_WORLD);

			step+=stride;
			openMsgCount++;
		}

		for (i = 1; i<=(openMsgCount); ++i) {
		/** Receive results from all worker processes in the connection. */
			MPI_Recv(&partSum,1,MPI_DOUBLE,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
			sum+=partSum;
		}
		openMsgCount = 0;
		stride = 1;
	}
	


	/** Inform all the worker nodes to quit and stop waiting for more work. */
	for (nextRank=1;nextRank<iNumProcs;nextRank++)
		MPI_Send(&nextRank,0,MPI_INT,nextRank,2,MPI_COMM_WORLD);

	return sum/(double)maxSteps;
}

int workerIntegral() {

	double xStart,y,xStep;
	MPI_Status status;
	int stride,i;
	while(1) {
		/* I am a worker, wait for work */

		/* get the status for the next incoming message */
		MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD, &status);
		/* if the  tag is 2, that the signal to stop */
		if (status.MPI_TAG==2) break;
		/* receive the left and right side of the trapezoid and compute the corresponding function values */

		y=0.0;
		MPI_Recv(&xStart,1,MPI_DOUBLE,0,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Recv(&xStep,1,MPI_DOUBLE,0,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Recv(&stride,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		
		for (i = 0; i < stride; ++i)
		{
			y+=function((double)(xStart+xStep*i));
		}

		/* send back the finished work */
		MPI_Send(&y,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);	

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
