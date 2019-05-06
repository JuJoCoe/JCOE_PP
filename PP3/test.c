#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>



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

void printArray (double *row, int nElements) {
    int i;
    for (i=0; i<nElements; i++) {
        printf("%f ", row[i]);
    }
    printf("\n");
}


int main(int argc, char **argv)
{
    double *A, *b, *x;
    int i,j,k;
    int index[2000];
    int N=2000;
    int myrank, numnodes, stripSize, offset, numElements;
    double startTime, endTime;


    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numnodes);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    N = atoi(argv[1]);


    //Allocate memory for matrix A (Memory allocation code received from Yong Chen)
	numElements = (N*N)/numnodes;

	if(myrank == 0){
	  A = createMatrix(N, N);
	//  printArray(A, N*N);
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

    	

   	   
    	for(i=0; i<N; i++)
    	    {
    	        index[i]= i % numnodes;
    	    }

    	    if (myrank == 0) {
    	    startTime = MPI_Wtime();

    	  }
    	    
    	    for(k=0;k<N;k++)
    	        {
    	            if (myrank == 0){
    	        	float y = A[k*k];
    	        	for(int j = k+1; j < N; j++){
    	        		A[k * j] = A[k * j]/y;
    	        		}
    	        		A[k * k] = 1.0;
    	        		b[k] = b[k]/y;



    	            }
    	            MPI_Scatter(A, numElements, MPI_DOUBLE, LocalA, numElements, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    	            MPI_Bcast (&b[k],1,MPI_DOUBLE,index[k],MPI_COMM_WORLD);

    	            for(i = 0; i<N; i++)
    	            {
    	                if(index[i] == myrank)
    	                {
			float z = LocalA[k];	
    	                    for(j=0;j<numElements;j++)
    	                    {
    	                    	LocalA[j] = LocalA[j] - z*LocalA[j];
    	                    	}

    	                    	b[i] = b[i] - LocalA[k] * b[k];
    	                    	LocalA[k] = 0.0;
    	                    }
    	                }
    	            MPI_Gather(LocalA, numElements, MPI_DOUBLE, A, numElements, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    	            }
    	    
    	    
    	    

    	        if (myrank == 0) {
    	        endTime = MPI_Wtime();
    	        printf("Time is %f\n", endTime-startTime);
    	      }


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
