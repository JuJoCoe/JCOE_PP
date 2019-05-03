#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char **argv)
{

    float A[500][500],b[500],multiplier[500],x[500];
    int i,j,k;
    int index[500];
    int N=250;
    int myrank, numnodes;
    double startTime, endTime;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);   /* get current process id */
    MPI_Comm_size(MPI_COMM_WORLD, &numnodes); /* get number of processes */
    MPI_Status status;

//////////////////////////////////////////////////////////////////////////////////

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

    MPI_Bcast (A,N*N,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast (b,N,MPI_DOUBLE,0,MPI_COMM_WORLD);

    for(i=0; i<N; i++)
    {
        index[i]= i % numnodes;
    }

    for(k=0;k<N;k++)
    {
        if (myrank == 0){
    	float y = A[k][k];
    	for(int j = k+1; j < N; j++){
    		A[k][j] = A[k][j]/y;
    		}
    		A[k][k] = 1.0;
    		b[k] = b[k]/y;
        }
        MPI_Bcast (&A[k][k],N-k,MPI_DOUBLE,index[k],MPI_COMM_WORLD);
        MPI_Bcast (&b[k],1,MPI_DOUBLE,index[k],MPI_COMM_WORLD);
 /*       for(i= k+1; i<N; i++)
        {
            if(index[i] == myrank)
            {
                multiplier[i]=A[i][k]/A[k][k];
            }
        }*/
        for(i= k+1; i<N; i++)
        {
        	float z = A[i][k];
            if(index[i] == myrank)
            {
                for(j=0;j<N;j++)
                {
                	A[i][j] = A[i][j] - z*A[k][j];
                				}

                	b[i] = b[i] - A[i][k] * b[k];
                	A[i][k] = 0.0;
                }
            }
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
