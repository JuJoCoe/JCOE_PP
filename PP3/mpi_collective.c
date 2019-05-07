#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>



//Create Matrix method, takes in a matrix and allocates memory
double *createMatrix (int nrows, int ncols){

    double *matrix;
    int h, i, j;

    if (( matrix = malloc(nrows*ncols*sizeof(double))) == NULL) {
        printf("Malloc error");
        exit(1);
    }

	srand(1);
    for (h=0; h<nrows*ncols; h++) {

        matrix[h] = (rand() % 11) - 5;
	    if(matrix[h] == 0){
    	 	matrix[h] = 1;
    		}
    }

    return matrix;
}

int main(int argc, char **argv)
{
    double *A, *b, *x;
    int i,j,k;
    int N=2000;
    int index[2000];
    int myrank, numnodes, stripSize, offset, numElements;
    double startTime, endTime;


    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numnodes);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    N = atoi(argv[1]);


    //numElements in 1-d array size
	numElements = (N*N)/numnodes;

	//create matrix A, only in rank 0
	if(myrank == 0){
	  A = createMatrix(N, N);
	}

	//Every process allocates LocalA
	 double *LocalA = malloc(sizeof(double) * numElements);



    //Allocate b to everyone
    	b = (double *) malloc (sizeof(double ) * N);
    	 if(b == NULL){
    		 printf("ERROR ALLOCATING b in cluster %s", processor_name);
    		 return -1;
    		 }
    	for(i = 0; i < N; i++){
    		b[i] = 1.0;
    	}


    	//Allocate x to only rank 0
    if(myrank == 0){
    x = (double *) malloc (sizeof(double ) * N);
    	if(x == NULL){
    		printf("ERROR ALLOCATING x in cluster %s", processor_name);
    		return -1;
    		}
    	for(i = 0; i < N; i++){
    		x[i] = 0.0;
    		}
    	}

    	//index to allow more consistent bcast operations
    	for(i=0; i<N; i++)
    	    {
    	        index[i]= i % numnodes;
    	    }

    	//start timer
    	if (myrank == 0) {
    	   startTime = MPI_Wtime();
    	  }

    	//Start Guassian Elimination
    	 for(k=0;k<N;k++){
    	    if (myrank == 0){
    	    float y = A[k*k];
    	    for(int j = k+1; j < N; j++){
    	       A[k * j] = A[k * j]/y;
    	    }
    	       A[k * k] = 1.0;
    	       b[k] = b[k]/y;
    	 }
    	 //Scatter A, each process gets numElements of A, All get b
    	 MPI_Scatter(A, numElements, MPI_DOUBLE, LocalA, numElements, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    	 MPI_Bcast (&b[k],1,MPI_DOUBLE,index[k],MPI_COMM_WORLD);

    	 //Parallelization
    	 for(i = 0; i < N/numnodes; i++){
    		 float z = LocalA[(i*N)+k];
    	     for(j=k+1;j<N;j++){
    	       LocalA[(i*N)+j] = LocalA[(i*N)+j] - z*LocalA[(i*N)+j];
    	       }

    	       b[k] = b[k] - LocalA[k] * b[k];
    	       LocalA[k] = 0.0;
    	  }

    	 //Send all work back to A in root = 0
    	  MPI_Gather(LocalA, numElements, MPI_DOUBLE, A, numElements, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }



   //End Timer and Print
   if (myrank == 0) {
    endTime = MPI_Wtime();
    printf("Time is %f\n", endTime-startTime);
   }

   //Backsub
   if (myrank==0){
	   for(int i=N-1; i >= 0; i--){
		   x[i] = b[i];
    	   for(int j=N-1; j > i; j--){
    		   x[i] = x[i] - A[i*j] * x[j];
    	   }
    	   x[i] = x[i]/A[i*i];
    	}
   }




    MPI_Finalize();
	return 0;
}
