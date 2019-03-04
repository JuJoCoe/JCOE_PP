#include <pthread.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#define MAX_THREADS 16


void *find_min_rw(void *list_ptr);

 int partial_list_size;
 int minimum_value = INT_MAX;
 int list_size = 100000000;
 int list_ptr[100000000];
 int num_threads = MAX_THREADS;
 int bounds = 0;


 typedef struct {
 int readers;
 int writer;
 pthread_cond_t readers_proceed;
 pthread_cond_t writer_proceed;
 int pending_writers;
 pthread_mutex_t read_write_lock;
 } mylib_rwlock_t;


 mylib_rwlock_t read_write_lock;

 void mylib_rwlock_init (mylib_rwlock_t *l) {
 l -> readers = l -> writer = l -> pending_writers = 0;
 pthread_mutex_init(&(l -> read_write_lock), NULL);
 pthread_cond_init(&(l -> readers_proceed), NULL);
 pthread_cond_init(&(l -> writer_proceed), NULL);
 }

 void mylib_rwlock_rlock(mylib_rwlock_t *l) {
 /* if there is a write lock or pending writers, perform condition
 wait.. else increment count of readers and grant read lock */

 pthread_mutex_lock(&(l -> read_write_lock));
 while ((l -> pending_writers > 0) || (l -> writer > 0))
 pthread_cond_wait(&(l -> readers_proceed),
 &(l -> read_write_lock));
 l -> readers ++;
 pthread_mutex_unlock(&(l -> read_write_lock));
 }


 void mylib_rwlock_wlock(mylib_rwlock_t *l) {
 /* if there are readers or writers, increment pending writers
 count and wait. On being woken, decrement pending writers
 count and increment writer count */

 pthread_mutex_lock(&(l -> read_write_lock));
 while ((l -> writer > 0) || (l -> readers > 0)) {
 l -> pending_writers ++;
 pthread_cond_wait(&(l -> writer_proceed),
 &(l -> read_write_lock));
 }
 l -> pending_writers --;
 l -> writer++;
 pthread_mutex_unlock(&(l -> read_write_lock));
 }


 void mylib_rwlock_unlock(mylib_rwlock_t *l) {
 /* if there is a write lock then unlock, else if there are
 read locks, decrement count of read locks. If the count
 is 0 and there is a pending writer, let it through, else
 if there are pending readers, let them all go through */

 pthread_mutex_lock(&(l -> read_write_lock));
 if (l -> writer > 0)
 l -> writer = 0;
 else if (l -> readers > 0)
 l -> readers --;
 pthread_mutex_unlock(&(l -> read_write_lock));
 if ((l -> readers == 0) && (l -> pending_writers > 0))
 pthread_cond_signal(&(l -> writer_proceed));
 else if (l -> readers > 0)
 pthread_cond_broadcast(&(l -> readers_proceed));
 }

 int main(int argc, char **argv) {



 	 //Thread intialization
 	 	 pthread_t p_threads[MAX_THREADS];
 	 	 pthread_attr_t attr;
 	 	 pthread_attr_init(&attr);

 	 //Populates the list with random numbers
 	 srand(time(0));
 	 for(int i = 0; i < list_size; i++){
 		 list_ptr[i] = rand();
 	 }

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

  // Replaced Min_Int with INT_MAX (INT_MAX is the max integer possible)
  minimum_value = INT_MAX;

  //divides the list size among the threads
  partial_list_size = list_size/num_threads;
  //Creates threads
  int i = 0;
  for (i = 0; i < num_threads; i++) {
  		pthread_create(&p_threads[i], &attr, find_min_rw, &list_ptr[bounds]);
  		bounds = bounds + partial_list_size;
  	}

  //Join threads so that the Total Min calculates the smallest min out of all threads
  for (int i = 0; i < num_threads; i++) {
  		pthread_join(p_threads[i], NULL);
  	}
 // Prints out Min
  printf("Total Min = %d\n", minimum_value);


  }







 void *find_min_rw(void *list_ptr) {

//intialize variables
 int *partial_list_pointer, my_min, i;
 my_min = INT_MAX;
 partial_list_pointer = (int *) list_ptr;

 //parses through entire list
 for (i = 0; i < partial_list_size; i++){
 if (partial_list_pointer[i] < my_min){
 my_min = partial_list_pointer[i];
 }
 }
 /* lock the mutex associated with minimum_value and
 update the variable as required */
 mylib_rwlock_rlock(&read_write_lock);
 if (my_min < minimum_value) {
 mylib_rwlock_unlock(&read_write_lock);
 mylib_rwlock_wlock(&read_write_lock);
 minimum_value = my_min;
 }
 /* and unlock the mutex */
 mylib_rwlock_unlock(&read_write_lock);
 //printf("This Thread Min = %d\n", my_min);
 pthread_exit(0);
  }
