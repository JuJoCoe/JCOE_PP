#include <pthread.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#define MAX_THREADS 16

 void *find_min(void *list_ptr);
 pthread_mutex_t minimum_value_lock;
 int minimum_value, partial_list_size;
 int list_size = 100000000;
 int list_ptr[100000000];
 int num_threads = MAX_THREADS;
 int bounds = 0;


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
	 pthread_t p_threads[MAX_THREADS];
	  	pthread_attr_t attr;
	  	pthread_attr_init(&attr);
	// srand(time(0));
	 for(int i = 0; i < list_size; i++){
		 list_ptr[i] = rand();
	 }

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

 /* declare and initialize data structures and list */
 minimum_value = INT_MAX;
 //pthread_init();
 pthread_mutex_init(&minimum_value_lock, NULL);
 partial_list_size = list_size/num_threads;
 for (int i = 0; i < num_threads; i++) {
 		pthread_create(&p_threads[i], &attr, find_min, &list_ptr[bounds]);
 		bounds = bounds + partial_list_size;
 	}

 for (int i = 0; i < num_threads; i++) {
 		pthread_join(p_threads[i], NULL);
 	}

 printf("Total Min = %d\n", minimum_value);

 //find_min(list_ptr);

 /* initialize lists, list_ptr, and partial_list_size */
 /* create and join threads here */
 }

 void *find_min(void *list_ptr) {

 int *partial_list_pointer, my_min, i;
 my_min = INT_MAX;
 partial_list_pointer = (int *) list_ptr;
 for (i = 0; i < partial_list_size; i++){
 if (partial_list_pointer[i] < my_min){
 my_min = partial_list_pointer[i];
 /* lock the mutex associated with minimum_value and
 update the variable as required */
 }}
 pthread_mutex_lock(&minimum_value_lock);
 if (my_min < minimum_value){
 minimum_value = my_min;
 }
 /* and unlock the mutex */
 pthread_mutex_unlock(&minimum_value_lock);
 printf("my_min = %d\n", my_min);
 pthread_exit(0);
}

