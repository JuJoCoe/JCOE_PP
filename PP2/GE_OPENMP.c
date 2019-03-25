#include <pthread.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


#define MAXN 2001
#define MAX_THREADS 8


float A[MAXN][MAXN];
float b[MAXN];
float x[MAXN];
int n = 2000;
float y = 0.0;
int num_threads = MAX_THREADS;

struct rc {
 int row;
 int innerrow;
 } rc_default = {0, 0};


 //Takes in arguments to declare number of threads (Code Reference: Yong Chen)
  int main(int argc, char **argv) {

	  if (argc != 2) {
	  	 		printf("Usage: %s <number of threads>\n", argv[0]);
	  	 		printf("Using a default number of threads: %d\n", MAX_THREADS);
	  	 		num_threads = MAX_THREADS;
	  	 	} else {
	  	 		num_threads = atoi(argv[1]);
	  	 		if (!num_threads || num_threads > MAX_THREADS) {
	  	 			printf("Incorrect number of threads provided\n");
	  	 			printf("Using a default number of threads: %d\n", MAX_THREADS);
	  	 			num_threads = MAX_THREADS;
	  	 		}
	  	 	}

//	 srand(time(0));
	 srand(1);
	 for(int i = 1; i <= n; i++){
	 		 for(int j = 1; j <= n; j++){
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




//start Gauss Elim


	for (int k = 1; k < n+1; k++){
			y = A[k][k];
			for(int j = k+1; j < n+1; j++){
				A[k][j] = A[k][j]/y;
			}
			b[k] = b[k]/y;
			A[k][k] = 1.0;
#pragma omp parallel for shared(A, b, n) num_threads(num_threads) schedule(static)
				for(int i = k+1; i<n+1; i++){
				float z = A[i][k];
						for(int l = k+1; l<n+1; l++){
							A[i][l] = A[i][l] - z*A[k][l];
						}
						b[i] = b[i] - A[i][k] * b[k];
						A[i][k] = 0.0;
			}

		}

	//run back substitution
	for(int i=n; i >= 1; i--){
		x[i] = b[i];
				for(int j=n; j > i; j--){
						x[i] = x[i] - A[i][j] * x[j];
				 }
				 x[i] = x[i]/A[i][i];
		}



if(n < 10){
	//Print outputs
	printf("\n");
	printf("X Outputs:\n");
	for(int l = 1; l <= n; l++){
		 printf("x%d = %f\n", l, x[l]);
	}
}
	return 0;
}


