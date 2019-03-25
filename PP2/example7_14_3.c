#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <limits.h>
#define MAX_THREADS 8

int i, j, k;
int dim;
int num_threads = MAX_THREADS;

float a[1000][1000];
float b[1000][1000];
float c[1000][1000];



int main(int argc, char **argv)
{
	dim = 1000;
//	 printf("\nEnter dimensions: ");
//	 scanf("%d",&dim);

	//Takes in the argument of the number of threads they want
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

	//Populates the 2 arrays using random integers.
	//Takes random integers between -5 and 5 for consistency and gets rid of all 0 values
	//because they make complicate multiplication
		 srand(time(0));
	//	 srand(1);
		for(int i = 0; i < dim; i++){
			 for(int j = 0; j < dim; j++){
				 a[i][j] = (rand() % 11) - 5;
			 			if(a[i][j] == 0){
			 				a[i][j] = 1;

			 			}

			 	b[i][j] = (rand() % 11) - 5;
			 		 	 if(b[i][j] == 0){
			 			 b[i][j] = 1;
			 		 }
			 		}
			 	 }


//Matrix multiplication algorithm received from the test book

//Parallelize all 3 loops using collapse method
#pragma omp parallel for shared(a, b, c, dim) num_threads(num_threads) schedule(static) collapse(3)
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			for (k = 0; k < dim; k++) {
				 c[i][j] += a[i] [k] * b[k][j];

			}
		}
	}

//Prints out results of matrix C
	if(dim < 8){
	for(int i = 0; i < dim; i++){
		for(int j = 0; j < dim; j++){
			printf("C[%d][%d] = %f\n", i, j, c[i][j]);
			}
		}
	}
}
