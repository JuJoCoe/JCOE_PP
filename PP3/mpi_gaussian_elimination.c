#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int i,j,k;
    int map[500];
    float A[500][500],b[500],c[500],x[500],sum=0.0;
    double range=1.0;
    int n=3;
    int rank, nprocs;
    clock_t begin1, end1, begin2, end2;
    MPI_Status status;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   /* get current process id */
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); /* get number of processes */

//////////////////////////////////////////////////////////////////////////////////

   if(rank == 0){
    						srand(1);
    						for(int i = 0; i < n; i++){
    							for(int j = 0; j < n; j++){ 	
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

//////////////////////////////////////////////////////////////////////////////////

    begin1 =clock();

    MPI_Bcast (A,n*n,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast (b,n,MPI_DOUBLE,0,MPI_COMM_WORLD);    

    for(i=0; i<n; i++)
    {
        map[i]= i % nprocs;
    } 

    for(k=0;k<n;k++)
    {
        MPI_Bcast (&A[k][k],n-k,MPI_DOUBLE,map[k],MPI_COMM_WORLD);
        MPI_Bcast (&b[k],1,MPI_DOUBLE,map[k],MPI_COMM_WORLD);
        for(i= k+1; i<n; i++) 
        {
            if(map[i] == rank)
            {
                c[i]=A[i][k]/A[k][k];
            }
        }               
        for(i= k+1; i<n; i++) 
        {       
            if(map[i] == rank)
            {
                for(j=0;j<n;j++)
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

    if (rank==0)
    { 
    x[n-1]=b[n-1]/A[n-1][n-1];
    for(i=n-2;i>=0;i--)
    {
        sum=0;

        for(j=i+1;j<n;j++)
        {
            sum=sum+A[i][j]*x[j];
        }
        x[i]=(b[i]-sum)/A[i][i];
    }

    end2 = clock();
    }
//////////////////////////////////////////////////////////////////////////////////
    if (rank==0)
    { 
        printf("\nThe solution is:");
        for(i=0;i<n;i++)
        {
            printf("\nx%d=%f\t",i,x[i]);

        }

        printf("\n\nLU decomposition time: %f", (double)(end1 - begin1) / CLOCKS_PER_SEC);
        printf("\nBack substitution time: %f\n", (double)(end2 - begin2) / CLOCKS_PER_SEC);
    }

    MPI_Finalize();
    return(0);

}
