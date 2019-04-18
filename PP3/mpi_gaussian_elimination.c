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

	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);


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
/*	for(i = 0; i < N; i++){
		b[i] = 1.0;
		printf("b[%d] = %f\n",i, b[i]);
	}
	*/

	//Allocate x
	x = (double *) malloc (sizeof(double ) * N);
		for(i = 0; i < N; i++){
			x[i] = 0.0;
		}

	if(myrank == 0){
		A[0][0] = 2;
		A[0][1] = 1;
		A[0][2] = -1;
		b[0] = 8;

		A[1][0] = -3;
		A[1][1] = -1;
		A[1][2] = 2;
		b[1] = -11;

		A[2][0] = -2;
		A[2][1] = 1;
		A[2][2] = 2;
		b[2] = -3;
	}

	for(i = 0; i < N; i++){
		for(j=0; j < N; j++){
			printf("A[%d][%d] = %f from node %s, rank %d\n", i, j, A[i][j], processor_name, myrank);
		}
		printf("b[%d] = %f",i, b[i]);
	}



























	MPI_Finalize();
	return 0;


}
