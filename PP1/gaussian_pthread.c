#include <pthread.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>


#define MAXN 2000
#define MAX_THREADS 8

float A[MAXN][MAXN];
float b[MAXN];
float x[MAXN];
int n = 200;
float y = 0.0;
int num_threads = MAX_THREADS;

clock_t start, end;
double cpu_time_used;


void *rowOps(void *ptr);
pthread_mutex_t lock;
pthread_cond_t m_cond;
int count = 0;
int threadcount = 1;


//Create a structure that contains row and innerrow values
struct rc {
 int row;
 int innerrow;
 } rc_default = {0, 0};

//Intialize Mutex locks
int
  pthread_mutex_init (
  pthread_mutex_t *mutex_lock,
  const pthread_mutexattr_t *lock_attr);

  int
  pthread_mutex_lock (
  pthread_mutex_t *mutex_lock);


  int
  pthread_mutex_unlock (
  pthread_mutex_t *mutex_lock);


  int main(int argc, char **argv) {

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

pthread_mutex_init(&lock, NULL);

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
		//Delete leftover threads from inner for loop
		if (threadcount >= 2){
		for (int z = 1; z <= threadcount-1; z++){
			count++;
			threadcount = 1;
			pthread_join(p_threads[z], NULL);
			}
		}

			y = A[k][k];
			for(int j = k+1; j < n+1; j++){
				A[k][j] = A[k][j]/y;
			}

			b[k] = b[k]/y;
			A[k][k] = 1.0;
			//row operations
			for(int i = k+1; i<n+1; i++){
				struct rc *thisrc = malloc(sizeof(struct rc));
				thisrc->row = k;
				thisrc->innerrow = i;

				//Creates thread calling ColumnsOps method and passing in struct arguement
				pthread_create(&p_threads[threadcount], &attr, rowOps, (void *)thisrc);
				//Keeps track of how many threads are created
				threadcount++;
		//		Concurrent thread limit reached, wait for threads to finish then move on
				if(threadcount == num_threads+1){
					for (int z = 1; z <= num_threads; z++){
						count++;
						threadcount = 1;
						pthread_join(p_threads[z], NULL);
						}

				}
			}
			}



//	Delete leftover threads
	for (int z = 1; z <= threadcount-1; z++){
		count++;
		pthread_join(p_threads[z], NULL);
	}

	//run back substitution
	for(int i=n; i >= 1; i--){
		x[i] = b[i];
				for(int j=n; j > i; j--){
						x[i] = x[i] - A[i][j] * x[j];
				 }
				 x[i] = x[i]/A[i][i];
		}



	//Print outputs
/*	printf("\n");
	printf("X Outputs:\n");
	for(int l = 1; l <= n; l++){
		 printf("x%d = %f\n", l, x[l]);
	}
*/


	return 0;
}


 //Method for row operations
void *rowOps(void *ptr){
	int innerrow, row = 0;
	struct rc *args = ptr;

	row = args->row;
	innerrow = args->innerrow;

			float z = A[innerrow][row];
			for(int l = row+1; l<n+1; l++){
				A[innerrow][l] = A[innerrow][l] - z*A[row][l];
			}
//			pthread_mutex_lock(&lock);
			b[innerrow] = b[innerrow] - A[innerrow][row] * b[row];
			A[innerrow][row] = 0.0;
//			pthread_mutex_unlock(&lock);



			pthread_exit(0);
}
