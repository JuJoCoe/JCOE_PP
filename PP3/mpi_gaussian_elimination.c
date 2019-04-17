#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAG 13

int main(int argc, char *argv[]) {
	double **A, **b, **x, **temp;
	int n = 2000;
	double startTime, endTime;
	int myrank, numnodes, stripsize, offset, numElements;

	MPI_Init(&argc, &argv);

	 MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	 MPI_Comm_size(MPI_COMM_WORLD, &numnodes);

	 n = atoi(argv[1]);

	 if (myrank == 0) {
	     temp = (double *) malloc (sizeof(double ) * n * n);
	     A = (double **) malloc (sizeof(double *) * n);
	     for (int i = 0; i < n; i++)
	       A[i] = &temp[i * n];
	   }
	   else {
	     temp = (double *) malloc (sizeof(double ) * n * n / numnodes);
	     A = (double **) malloc (sizeof(double *) * n / numnodes);
	     for (int i = 0; i < n / numnodes; i++)
	       A[i] = &temp[i * n];
	   }

	   temp = (double *) malloc (sizeof(double ) * n * n);
	   b = (double **) malloc (sizeof(double *) * n);
	   for (int i = 0; i < n; i++)
	     b[i] = &temp[i * n];


	   if (myrank == 0) {
	     temp = (double *) malloc (sizeof(double ) * n * n);
	     x = (double **) malloc (sizeof(double *) * n);
	     for (int i = 0; i < n; i++)
	       x[i] = &temp[i * n];
	   }
	   else {
	     temp = (double *) malloc (sizeof(double ) * n * n / numnodes);
	     x = (double **) malloc (sizeof(double *) * n / numnodes);
	     for (int i = 0; i < n / numnodes; i++)
	       x[i] = &temp[i * n];
	   }

	   if (myrank == 0) {
	      // initialize A and B
		    A[1][1] = 2;
		   	A[1][2] = 1;
		   	A[1][3] = -1;
		   	b[1] = 8;

		   	A[2][1] = -3;
		   	A[2][2] = -1;
		   	A[2][3] = 2;
		   	b[2] = -11;

		   	A[3][1] = -2;
		   	A[3][2] = 1;
		   	A[3][3] = 2;
		   	b[3] = -3;
	        }

	   // start timer
	     if (myrank == 0) {
	       startTime = MPI_Wtime();
	     }

	     stripsize = n/numnodes;
	     for (int k = 1; k < n+1; k++){
	     		//Delete leftover threads from inner for loop
	     			float y = A[k][k];
	     			for(int j = k+1; j < n+1; j++){
	     				A[k][j] = A[k][j]/y;
	     			}

	     			b[k] = b[k]/y;
	     			A[k][k] = 1.0;

	     // send each node its piece of A -- note could be done via MPI_Scatter
	       if (myrank == 0) {
	         offset = stripsize;
	         numElements = stripsize * n;
	         for (int i=1; i<numnodes; i++) {
	           MPI_Send(A[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
	           offset += stripsize;
	         }
	       }
	       else {  // receive my part of A
	         MPI_Recv(A[0], stripsize * n, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	       }

	       // everyone gets B
	       MPI_Bcast(b[0], n*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);


	       for(int s = k+1; s<n+1; s++){
	       			float z = A[s][k];
	       			for(int l = k+1; l<n+1; l++){
	       				A[s][l] = A[s][l] - z*A[k][l];
	       			}
	       			b[s] = b[s] - A[s][k] * b[k];
	       			A[s][k] = 0.0;
	       	}




	       if (myrank == 0) {
	           offset = stripsize;
	           numElements = stripsize * n;
	           for (int i=1; i<numnodes; i++) {
	             MPI_Recv(A[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	             offset += stripsize;
	           }
	         }
	         else { // send my contribution to C
	           MPI_Send(A[0], stripsize * n, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
	         }

	       if (myrank == 0) {
	       	           offset = stripsize;
	       	           numElements = stripsize * n;
	       	           for (int i=1; i<numnodes; i++) {
	       	             MPI_Recv(b[offset], numElements, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	       	             offset += stripsize;
	       	           }
	       	         }
	       	         else { // send my contribution to C
	       	           MPI_Send(b[0], stripsize * n, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
	       	         }



	       if (myrank == 0) {
	           endTime = MPI_Wtime();
	           printf("Time is %f\n", endTime-startTime);
	         }

	         // print out matrix here, if I'm the master
	         if (myrank == 0 && n < 10) {
	           for (int i=0; i<n; i++) {
	               printf("%f ", x[i]);
	             printf("\n");
	           }
	         }

	         MPI_Finalize();
	         return 0;




	     }



}
