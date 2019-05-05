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
	int number = 0;
	double y;
	int leftover;
	int indexrow;
	int size;
	int count = 0;

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
	    if(tmp == NULL){
	    	printf("ERROR ALLOCATING");
	    	 return -1;
	    	    }
	    if(A == NULL){
	    	printf("ERROR ALLOCATING 2");
	    	return -1;
	    }
	    for (i = 0; i < N; i++)
	      A[i] = &tmp[i * N];
	  }
	  else {
	    tmp = (double *) malloc (sizeof(double ) * ((N * N / numnodes)+1));
	    A = (double **) malloc (sizeof(double *) * ((N / numnodes)+1));
	    if(tmp == NULL){
	   	    	    	printf("ERROR ALLOCATING 3");
	   	    	    	return -1;
	   	    	    }
	    if(A == NULL){
	    	printf("ERROR ALLOCATING 4");
	    	return -1;
	    	    }
	    for (i = 0; i < N / numnodes; i++)
	      A[i] = &tmp[i * N];
	  }

	//Allocate b
	b = (double *) malloc (sizeof(double ) * N);
	 if(b == NULL){
		    	    	printf("ERROR ALLOCATING b");
		    	    	return -1;
		    	    }
	for(i = 0; i < N; i++){
		b[i] = 1.0;
	}


	//Allocate x
	x = (double *) malloc (sizeof(double ) * N);
	 if(x == NULL){
		    	    	printf("ERROR ALLOCATING x");
		    	    	return -1;
		    	    }
		for(i = 0; i < N; i++){
			x[i] = 0.0;
		}

	//Populate Matrix
	if(myrank == 0){
		srand(1);
		for(int i = 0; i < N; i++){
			for(int j = 0; j < N; j++){
			 	A[i][j] = (rand() % 11) - 5;
			 	if(A[i][j] == 0){
			 		A[i][j] = 1;
			 			}
			 		 }
			 		 b[i] = rand() % (10 + 1 - 0) + 0;
			 		 if(b[i] == 0){
			 			 b[i] = 1;
			 		 }
			 	 	 }

		MPI_Bcast(&b[0], N, MPI_INT, 0, MPI_COMM_WORLD);
	}


	if (myrank == 0) {
    		startTime = MPI_Wtime();
	  }

	for(k = 0; k < N ; k++){

		if (myrank == 0){
			y = A[k][k];
			for(int j = k+1; j < N; j++){
				A[k][j] = A[k][j]/y;
			}
			A[k][k] = 1.0;
			b[k] = b[k]/y;
			MPI_Bcast(&b[0], N, MPI_INT, 0, MPI_COMM_WORLD);
		}




	stripSize = N/numnodes;

  // send each node its piece of A -- note could be done via MPI_Scatter
 	 if (myrank == 0) {
		printf("N = %d, numnodes = %d, stirpSize = %d\n", N, numnodes, stripSize); 
    		offset = stripSize;
    		numElements = stripSize * N;
    		for (i=1; i<numnodes; i++) {
			indexrow = offset;
			MPI_Send(&indexrow, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
      			MPI_Send(A[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
      			offset += stripSize;
    		}
  	}
  	else {  // receive my part of A
		MPI_Recv(&indexrow, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    		MPI_Recv(A[0], stripSize * N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  	}

	for(int s = 0; s<stripSize; s++){
		float z = A[s][k];
		int total = sizeof(A);
		int row = sizeof(A[0]);
		int testsize = sizeof(A)/sizeof(A[0]);
		printf("A has %d rows, total = %d, row = %d\n", testsize, total, row);
		for(int l = k+1; l<N; l++){
			A[s][l] = A[s][l] - z*A[s][l];
			//printf("Printed HERE\n");
		}
			b[s] = b[s] - A[s][k] * b[k];
			A[s][k] = 0.0;
		}


		

if (myrank == 0) {
   offset = stripSize;
   numElements = stripSize * N;
   for (i=1; i<numnodes; i++) {
     MPI_Recv(A[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
     offset += stripSize;
   }
 }
 else { // send my contribution to C
   MPI_Send(A[0], stripSize * N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
 }

	}
if (myrank == 0) {
    endTime = MPI_Wtime();
    printf("Time is %f\n", endTime-startTime);
  }
	
if(myrank == 0){
		for(int i=N-1; i >= 0; i--){
			x[i] = b[i];
			for(int j=N-1; j > i; j--){
				x[i] = x[i] - A[i][j] * x[j];
				}
			  	x[i] = x[i]/A[i][i];
		//	printf("x%d = %f\n", i, x[i]);
			}

	}






























	MPI_Finalize();
	return 0;


}
