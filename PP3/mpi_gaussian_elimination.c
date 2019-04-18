#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAG 13

int main(int argc, char *argv[]) {
	double **A, *b, *x, *tmp;
	int N = 2000;
	double startTime, endTime;
	int myrank, numnodes, stripSize, offset, numElements;
	int i, j, k;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &numnodes);

	N = atoi(argv[1]);

	//Allocate A
	if (myrank == 0) {
	    tmp = (double *) malloc (sizeof(double ) * N * N);
	    A = (double **) malloc (sizeof(double *) * N);
	    for (i = 0; i < N; i++)
	      A[i] = &tmp[i * N];
	  }
	  else {
	    tmp = (double *) malloc (sizeof(double ) * N * N / numnodes);
	    A = (double **) malloc (sizeof(double *) * N / numnodes);
	    for (i = 0; i < N / numnodes; i++)
	      A[i] = &tmp[i * N];
	  }

	//Allocate b
	b = (double *) malloc (sizeof(double ) * N);
	for(i = 0; i < N; i++){
		b[i] = 1;
		printf("b[%d] = %d\n",i, b[i]);
	}

	MPI_Finalize();
	return 0;


}

