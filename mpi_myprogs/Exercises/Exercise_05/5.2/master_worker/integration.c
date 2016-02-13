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


double integral(double (*function)(double x),double x_start,double x_end,int maxSteps)
{
	int iMyRank,iNumProcs;
	MPI_Comm_rank(MPI_COMM_WORLD,&iMyRank);
	MPI_Comm_size(MPI_COMM_WORLD,&iNumProcs);

	double sum=0.0;
	double x[2],y[2];
	MPI_Status status;
	double stepSize=(x_end-x_start)/(double)maxSteps;
	int step;
	int nextRank=1;
	if (iMyRank==0)	
	{
		/* I am the master, distribute the work */
		for (step=0;step<maxSteps;step++)
		{

			x[0]=x_start+stepSize*step;
			x[1]=x_start+stepSize*(step+1);
			nextRank=step%(iNumProcs-1)+1;
			MPI_Send(x,2,MPI_DOUBLE,nextRank,0,MPI_COMM_WORLD);
			MPI_Recv(y,2,MPI_DOUBLE,nextRank,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			sum+=stepSize*0.5*(y[0]+y[1]);
		}
		for (nextRank=1;nextRank<iNumProcs;nextRank++)
			MPI_Send(&nextRank,0,MPI_INT,nextRank,2,MPI_COMM_WORLD);
	}
	else while(1)

	{
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
	return sum;
}



int main(int argc,char** argv)
{
	double x0=0.0,x1=1.0;
	int maxSteps=1000;
	double stepSize=(x1-x0)/maxSteps;
	double halpStep=stepSize/2.0;
	MPI_Init(&argc,&argv);
	double startTime,stopTime;
	startTime=MPI_Wtime();
	int iMyRank,iNumProcs;
	MPI_Comm_rank(MPI_COMM_WORLD,&iMyRank);
	MPI_Comm_size(MPI_COMM_WORLD,&iNumProcs);	
	
	double pi=integral(function,0.0,1.0,100);
	
	stopTime=MPI_Wtime();
	printf("PI = %lf\t approximation = %lf\nComputation took %lf seconds\n",M_PI,pi,stopTime-startTime);
	MPI_Finalize();
	
}
