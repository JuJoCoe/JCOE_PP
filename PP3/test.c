#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char **argv)
{

    double **A, **LocalA, *b, *x, *tmp;
    int i,j,k;
    int index[250];
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
	int from = myrank * N;

    //Allocate memory for matrix A (Memory allocation code received from Yong Chen)
	
	if(myrank == 0){
	    tmp = (double *) malloc (sizeof(double ) * N * N);
	    A = (double **) malloc (sizeof(double *) * N);
	    if(tmp == NULL){
	    	printf("ERROR ALLOCATING tmp in rank 0");
	    	return -1;
	    }	
	    if(A == NULL){
	    	printf("ERROR ALLOCATING A in rank 0");
	    	return -1;
	    }

	    for (i = 0; i < N; i++)
	      A[i] = &tmp[i * N];
	}
	//Every process allocates LocalA
	    tmp = (double *) malloc (sizeof(double ) * ((N * N / numnodes)+1));
	    LocalA = (double **) malloc (sizeof(double *) * ((N / numnodes)+1));
	    if(tmp == NULL){
	    	printf("ERROR ALLOCATING tmp in cluster %s", processor_name);
	    	return -1;
	   	   	}
	    if(LocalA == NULL){
	    	printf("ERROR ALLOCATING A in cluster %s", processor_name);
	    	return -1;
	    	}
	    for (i = 0; i < N / (numnodes+1); i++)
	      LocalA[i] = &tmp[i * N];
	  
	  

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
   }
	for(int i = 0; i < N; i++){
    		for(int j = 0; j < N; j++){
			LocalA[i][j] == 0;
	//		printf("LocalA = %f\n", LocalA[0][0]);
		}
	}
	
	
	stripSize = N/numnodes;
	numElements = stripSize;

	
   MPI_Scatter(A, numElements, MPI_DOUBLE, LocalA[0], numElements, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	if(myrank == 0){
	printf("A[0][0] = %f\n", A[1][0]);	
	}else{
	printf("LocalA[0][0] = %f\n", LocalA[0][0]);
	}
	
    MPI_Finalize();
	return 0;
}
