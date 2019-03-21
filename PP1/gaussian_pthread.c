#include <pthread.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>


#define MAXN 2001
#define MAX_THREADS 8

//Initialize Variables
float A[MAXN][MAXN];
float b[MAXN];
float x[MAXN];
int n = 1000;
float y = 0.0;
int num_threads = MAX_THREADS;

//creates a clock variable, this was used to time certain aspects of the code (currently commented out)
clock_t start, end;
double cpu_time_used;

//Row Operation method
void *rowOps(void *ptr);

//keeps track of the pthread index
int threadcount = -1;

//keeps track of what row to start on
int indexrow = 1;

//Create a structure that contains row and innerrow values
struct rc {
 int row;
 int innerrow;
 int iterations;
 int leftover;
 } rc_default = {0, 0, 0, 0};


 int main(int argc, char **argv) {

	 //allows user to input since of the array
	  printf("\nEnter integer n to create a nxn matrix: ");
	  scanf("%d",&n);

	  //Takes in arguments to declare number of threads (Code Reference: Yong Chen)
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

//Intialize mutex lock
//pthread_mutex_init(&lock, NULL);

//Generate the nxn matrix using random number function
	 srand(time(0));
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


    //initialize threads
	pthread_t p_threads[num_threads];
	pthread_attr_t attr;
	pthread_attr_init(&attr);



//start Gauss Elim
	for (int k = 1; k < n+1; k++){
		threadcount=-1;
		//Delete leftover threads from inner for loop
			y = A[k][k];
			for(int j = k+1; j < n+1; j++){
				A[k][j] = A[k][j]/y;
			}

			b[k] = b[k]/y;
			A[k][k] = 1.0;


			//Calculates total number of times the inner loop will run
			int TotalIterations = n - (k);
			//Calculates the total number of iteration each thread will run
			int IterationsPerThread = TotalIterations/num_threads;
			//Left over iterations that will be given to some threads
			int Remainder = TotalIterations%num_threads;
			//Which indexrow to start at
			indexrow = k+1;


			//Multithreading
			for (int i = 0; i < num_threads; i++) {
				int leftover = 0;
				//struct to pass in for threads
				struct rc *thisrc = malloc(sizeof(struct rc));
				thisrc->row = k;
				thisrc->innerrow = indexrow;

				//while there is still a remainder left, add 1 to thread iteration
				if(Remainder != 0){
					Remainder--;
					leftover++;
				}
				//adds to struct
				thisrc->iterations = IterationsPerThread;
				thisrc->leftover = leftover;

				//This is to check to see if we need to use each thread(if loop only runs 6 times use only 6 threads)
				if(indexrow < IterationsPerThread +leftover+indexrow){
					pthread_create(&p_threads[i], &attr, rowOps, (void *)thisrc);
					//moves the rows
					indexrow = indexrow + IterationsPerThread + leftover;
					//keeps track of how many threads are created(just incase we only create a few threads)
					threadcount++;
				}

			}

			//Join threads
			for (int i = 0; i <= threadcount; i++){
				pthread_join(p_threads[i], NULL);
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

//Prints out anything lower than 11 dimensions to check for accuary, over 11 would be to much
if(n < 11){
	//Print outputs
	printf("\n");
	printf("X Outputs:\n");
	for(int l = 1; l <= n; l++){
		 printf("x%d = %f\n", l, x[l]);
	}
}


	return 0;
}


 //Method for row operations
void *rowOps(void *ptr){
	int innerrow, row, iterations, leftover = 0;
	struct rc *args = ptr;

	row = args->row;
	innerrow= args->innerrow;
	iterations=args->iterations;
	leftover=args->leftover;

	for(int s = innerrow; s<iterations+innerrow+leftover; s++){
			float z = A[s][row];
			for(int l = row+1; l<n+1; l++){
				A[s][l] = A[s][l] - z*A[row][l];
			}
			b[s] = b[s] - A[s][row] * b[row];
			A[s][row] = 0.0;
	}
			pthread_exit(0);
}


