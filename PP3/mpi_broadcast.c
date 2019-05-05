#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char **argv)
{

    double **A, *b, *x, *tmp;
    int i,j,k;
    int index[500];
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
    	if (myrank == 0) {
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
    	  }else{
    		  tmp = (double *) malloc (sizeof(double ) * ((N * N / numnodes)+1));
    		   A = (double **) malloc (sizeof(double *) * ((N / numnodes)+1));
    		   if(tmp == NULL){
    		  	  	printf("ERROR ALLOCATING tmp in cluster %s", processor_name);
    		  	   	return -1;
    		  	}
    		  	if(A == NULL){
    		  		printf("ERROR ALLOCATING A in cluster %s", processor_name);
    		  	    return -1;
    		  	}
    		  	for (i = 0; i < N / numnodes; i++)
    		  	  A[i] = &tmp[i * N];
    	  }

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

   
    MPI_Bcast (&b[0],N,MPI_DOUBLE,0,MPI_COMM_WORLD);

    for(i=0; i<N; i++)
    {
        index[i]= i % numnodes;
    }

    if (myrank == 0) {
    startTime = MPI_Wtime();
    
  }

    for(k=0;k<N;k++)
    {
    	stripSize = N/numnodes;
    	numElements = stripSize * N;
        if (myrank == 0){
    	float y = A[k][k];
    	for(int j = k+1; j < N; j++){
    		A[k][j] = A[k][j]/y;
    		}
    		A[k][k] = 1.0;
    		b[k] = b[k]/y;
    		
        }
	MPI_Scatter(A, numElements, MPI_DOUBLE, A, numElements, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast (&b[k],1,MPI_DOUBLE,index[k],MPI_COMM_WORLD);
 
        for(i= k+1; i<N; i++)
        {
        	float z = A[i][k];
            if(index[i] == myrank)
            {
                for(j=0;j<N;j++)
                {
                	A[i][j] = A[i][j] - z*A[i][j];
                				}

                	b[i] = b[i] - A[i][k] * b[k];
                	A[i][k] = 0.0;
                }
            }
        }

    if (myrank == 0) {
    endTime = MPI_Wtime();
    printf("Time is %f\n", endTime-startTime);
  }


    if (myrank==0){

    	for(int i=N-1; i >= 0; i--){
    		x[i] = b[i];
    		for(int j=N-1; j > i; j--){
    			x[i] = x[i] - A[i][j] * x[j];
    			}
    			x[i] = x[i]/A[i][i];
    			}
    	}

    MPI_Finalize();
    return(0);

}
