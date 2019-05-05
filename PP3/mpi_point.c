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
	    		number = indexrow;
	    		int size = N * (IterationsPerThread + leftover);
	    		MPI_Send(&number, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
	    		MPI_Send(&size, 1, MPI_INT, i, TAG+1, MPI_COMM_WORLD);
	    		MPI_Send(A[indexrow], size, MPI_DOUBLE, i, TAG+2, MPI_COMM_WORLD);
	    		indexrow = indexrow + IterationsPerThread + leftover;
	    	}else{
	    		number = 0;
	    		int size = 1;
	    		MPI_Send(&number, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
	    		MPI_Send(&size, 1, MPI_INT, i, TAG+1, MPI_COMM_WORLD);
	    		MPI_Send(A[0], 1, MPI_DOUBLE, i, TAG+2, MPI_COMM_WORLD);
	    	}
	    	}
	    }
		else {  // receive my part of A
		MPI_Recv(&number, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&size, 1, MPI_INT, 0, TAG+1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	        MPI_Recv(A[0], size, MPI_DOUBLE, 0, TAG+2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		  }

	if(number != 0 && myrank != 0){
		for(int s = 0; s<(size/N); s++){
		//	printf("k = %d\n" , k);
			float z = A[s][k];
			for(int l = k+1; l<N; l++){
				A[s][l] = A[s][l] - z*A[s][l];
			}

				b[number] = b[number] - A[s][k] * b[k];
				A[s][k] = 0.0;

			}

	}
		
	if(numnodes == 1){
		for(int s = 0; s<(N-(k+1)); s++){
		//	printf("k = %d\n" , k);
			float z = A[s][k];
			for(int l = k+1; l<N; l++){
				A[s][l] = A[s][l] - z*A[k][l];
			}

				b[number] = b[number] - A[s][k] * b[k];
				A[s][k] = 0.0;

			}
	}
		


	//	printf("Completed row operations %d\n", k);
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
			    		int size = N * (IterationsPerThread + leftover);
			    		int bsize = size/3;
			    		MPI_Recv(A[indexrow], size, MPI_DOUBLE, i, TAG+4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			    		MPI_Recv(&b[indexrow], bsize, MPI_DOUBLE, i, TAG+5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			    		indexrow = indexrow + IterationsPerThread + leftover;
			    	}
			    }
	}else{
		if(number != 0){
		int bsize = size/3;
		MPI_Send(A[0], size, MPI_DOUBLE, 0, TAG+4, MPI_COMM_WORLD);
		MPI_Send(&b[0], bsize, MPI_DOUBLE, 0, TAG+5, MPI_COMM_WORLD);
		}
	}





//	 MPI_Barrier(MPI_COMM_WORLD);
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


	if (myrank == 0) {
    endTime = MPI_Wtime();
    printf("Time is %f\n", endTime-startTime);
  }



























	MPI_Finalize();
	return 0;


}
