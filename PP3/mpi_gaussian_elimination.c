#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAG 13

int main(int argc, char *argv[]) {
	double **A, *b, *x, *tmp;
	int map[3];
	int C[3];
	int N = 2000;
	double startTime, endTime;
	int myrank, numnodes, stripSize, offset, numElements;
	int i, j, k;
	int number = 0;
	double y;
	int indexrow;

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

	if(myrank == 0){
		A[0][0] = 2;
		A[0][1] = 1;
		A[0][2] = -1;
		b[0] = 8;

		A[1][0] = -3;
		A[1][1] = -1;
		A[1][2] = 2;
		b[1] = -11;

		A[2][0] = -2;
		A[2][1] = 1;
		A[2][2] = 2;
		b[2] = -3;
	}

		b[0] = 8;
		b[1] = -11;
		b[2] = -3;



	//	 begin1 =clock();

		    MPI_Bcast (A[0],9,MPI_DOUBLE,0,MPI_COMM_WORLD);
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
		                C[i]=A[i][k]/A[k][k];
		            }
		        }
		        for(i= k+1; i<N; i++)
		        {
		            if(map[i] == myrank)
		            {
		                for(j=0;j<N;j++)
		                {
		                    A[i][j]=A[i][j]-( C[i]*A[k][j] );
		                }
		                b[i]=b[i]-( C[i]*b[k] );
		            }
		        }
		    }
		 //   end1 = clock();

		//////////////////////////////////////////////////////////////////////////////////

	//	    begin2 =clock();

		    if (myrank==0)
		    {
		    x[N-1]=b[N-1]/A[N-1][N-1];
		    for(i=N-2;i>=0;i--)
		    {
		        double sum=0.0;

		        for(j=i+1;j<N;j++)
		        {
		            sum=sum+A[i][j]*x[j];
		        }
		        x[i]=(b[i]-sum)/A[i][i];
		    }

	//	    end2 = clock();
		    }
		//////////////////////////////////////////////////////////////////////////////////
		    if (myrank==0){
		        printf("\nThe solution is:");
		        for(i=0;i<N;i++)
		        {
		            printf("\nx%d=%f\t",i,x[i]);

		        }
		    }







    MPI_Finalize();
	return 0;


}
