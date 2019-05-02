#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

 //   double **A, *b, *x, *c, *tmp;
    int i,j,k;
 //   int sum;
    int map[2000];
    float A[1000][1000],b[1000],c[1000],x[1000],sum=0.0;
    double range=1.0;
    int N=2000;
    int myrank, numnodes;
    clock_t begin1, end1, begin2, end2;
    MPI_Status status;

    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);   /* get current process id */
    MPI_Comm_size(MPI_COMM_WORLD, &numnodes); /* get number of processes */

//////////////////////////////////////////////////////////////////////////////////
/*
    //Allocate A
    	if (myrank == 0) {
    	    tmp = (double *) malloc (sizeof(double ) * N * N);
    	    A = (double **) malloc (sizeof(double *) * N);
    	    for (i = 0; i < N; i++)
    	      A[i] = &tmp[i * N];
    	  }else{
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
*/
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
//////////////////////////////////////////////////////////////////////////////////

    begin1 =clock();

    MPI_Bcast (A,N*N,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast (b,N,MPI_DOUBLE,0,MPI_COMM_WORLD);

    for(i=0; i<N; i++)
    {
        map[i]= i % numnodes;
    }

    for(k=0;k<N;k++)
    {
        MPI_Bcast (&A[k][k],N-k,MPI_DOUBLE,map[k],MPI_COMM_WORLD);
        MPI_Bcast (&b[k],1,MPI_DOUBLE,map[k],MPI_COMM_WORLD);
        for(i= k+1; i<N; i++)
        {
            if(map[i] == myrank)
            {
                c[i]=A[i][k]/A[k][k];
            }
        }
        for(i= k+1; i<N; i++)
        {
            if(map[i] == myrank)
            {
                for(j=0;j<N;j++)
                {
                    A[i][j]=A[i][j]-( c[i]*A[k][j] );
                }
                b[i]=b[i]-( c[i]*b[k] );
            }
        }
    }
    end1 = clock();

//////////////////////////////////////////////////////////////////////////////////

    begin2 =clock();

    if (myrank==0)
    {
    x[N-1]=b[N-1]/A[N-1][N-1];
    for(i=N-2;i>=0;i--)
    {
        sum=0;

        for(j=i+1;j<N;j++)
        {
            sum=sum+A[i][j]*x[j];
        }
        x[i]=(b[i]-sum)/A[i][i];
    }

    end2 = clock();
    }
//////////////////////////////////////////////////////////////////////////////////
    if (myrank==0)
    {
        printf("\nThe solution is:");
        for(i=0;i<N;i++)
        {
      //      printf("\nx%d=%f\t",i,x[i]);

        }

        printf("\n\nLU decomposition time: %f", (double)(end1 - begin1) / CLOCKS_PER_SEC);
        printf("\nBack substitution time: %f\n", (double)(end2 - begin2) / CLOCKS_PER_SEC);
    }


    MPI_Finalize();
    return(0);
}
