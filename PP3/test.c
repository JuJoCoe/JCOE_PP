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
	for(i = 0; i < N; i++){
		b[i] = 1.0;
	}


	//Allocate x
	x = (double *) malloc (sizeof(double ) * N);
		for(i = 0; i < N; i++){
			x[i] = 0.0;
		}

	//Populate Matrix
	if(myrank == 0){
		A[0][0] = 2;
		A[0][1] = 1;
		A[0][2] = -1;

		A[1][0] = -3;
		A[1][1] = -1;
		A[1][2] = 2;

		A[2][0] = -2;
		A[2][1] = 1;
		A[2][2] = 2;
	}

	b[0] = 8;
	b[1] = -11;
	b[2] = -3;


	//numnodes = number of processes
	stripSize = N/numnodes;


	for(k = 0; k < N ; k++){

		if (myrank == 0){
			y = A[k][k];
			for(int j = k+1; j < N; j++){
				A[k][j] = A[k][j]/y;
			}
			A[k][k] = 1.0;
		}
		b[k] = b[k]/y;



	if (myrank == 0) {

		//Calculates total number of times the inner loop will run
		int TotalIterations = N - (k+1);
					//Calculates the total number of iteration each thread will run
		int IterationsPerThread = TotalIterations/numnodes;
					//Left over iterations that will be given to some threads
		int Remainder = TotalIterations%numnodes;
					//Which indexrow to start at
		indexrow = k+1;


	    for (i=1; i<numnodes; i++) {
		leftover = 0;
		    
	    	if(Remainder != 0){
	    		Remainder--;
	    		leftover++;
	    	}

	    	if(indexrow < IterationsPerThread +leftover+indexrow){
	    		number = 1;
	    		int size = N * (IterationsPerThread + leftover);
			printf("sending size = %d\n", size);
	    		MPI_Send(&number, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
	    		MPI_Send(&size, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
	    		MPI_Send(A[indexrow], size, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
	    		indexrow = indexrow + IterationsPerThread + leftover;
	    	}else{
	    		number = 0;
	    		int size = 1;
	    		MPI_Send(&number, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
	    		MPI_Send(&size, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
	    		MPI_Send(A[0], 1, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
	    	}
		    printf("Sent first time %d\n", k); 
	    	}
	    }
		else {  // receive my part of A
		MPI_Recv(&number, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&size, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("number and size = %d and %d from node %s, rank %d\n", number, size, processor_name, myrank);	
	    MPI_Recv(A[0], size, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("Recieved first time %d\n", k); 
		  }


	MPI_Barrier(MPI_COMM_WORLD);
	if(number == 1){
		A[0][0] = 1000 + k;
	}

	MPI_Barrier(MPI_COMM_WORLD);
	if(myrank == 0){
		//Calculates total number of times the inner loop will run
				int TotalIterations = N - (k+1);
							//Calculates the total number of iteration each thread will run
				int IterationsPerThread = TotalIterations/numnodes;
							//Left over iterations that will be given to some threads
				int Remainder = TotalIterations%numnodes;
							//Which indexrow to start at
				indexrow = k+1;


			    for (i=1; i<numnodes; i++) {
				leftover = 0;

			    	if(Remainder != 0){
			    		Remainder--;
			    		leftover++;
			    	}

			    	if(indexrow < IterationsPerThread +leftover+indexrow){
			    		number = 1;
			    		int size = N * (IterationsPerThread + leftover);
			    		MPI_Recv(A[indexrow], size, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			    		indexrow = indexrow + IterationsPerThread + leftover;
			    	}
				    printf("Recieved Second time %d\n", k); 
			    }
	}else{
		if(number == 1){
		MPI_Send(A[0], size, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
			printf("Sent Second time %d\n", k); 
		}
	}

	if(myrank == 0){
		for(i = 0; i < N; i++){
				for(j=0; j < N; j++){
					printf("A[%d][%d] = %f from node %s, rank %d\n", i, j, A[i][j], processor_name, myrank);
				}
		}
	}





	 MPI_Barrier(MPI_COMM_WORLD);
	}



/*
	if(myrank == 0){
	for(i = 0; i < N; i++){
		for(j=0; j < N; j++){
			printf("A[%d][%d] = %f from node %s, rank %d\n", i, j, A[i][j], processor_name, myrank);
//			printf("number = %d from node %s, rank %d\n", number, processor_name, myrank);
		}
//		printf("b[%d] = %f from node %s, rank %d\n",i, b[i], processor_name, myrank);
	}
	}
	else {
		for(i = 0; i < 1; i++){
			for(j=0; j < N; j++){
				printf("A[%d][%d] = %f from node %s, rank %d\n", i, j, A[i][j], processor_name, myrank);
//				printf("number = %d from node %s, rank %d\n", number, processor_name, myrank);
			}
//			printf("b[%d] = %f from node %s, rank %d\n",i, b[i], processor_name, myrank);
		}
	}

*/
























	MPI_Finalize();
	return 0;


}
