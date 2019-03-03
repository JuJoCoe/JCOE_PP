#include <pthread.h>
#include <stdio.h>
#include <limits.h>

 void *find_min(void *list_ptr);
 pthread_mutex_t minimum_value_lock;
 int minimum_value, partial_list_size;

 main() {
 /* declare and initialize data structures and list */
 minimum_value = INT_MIN;
 //pthread_init();
 pthread_mutex_init(&minimum_value_lock, NULL);

 /* initialize lists, list_ptr, and partial_list_size */
 /* create and join threads here */
 }

 void *find_min(void *list_ptr) {
 int *partial_list_pointer, my_min, i;
 my_min = INT_MIN;
 partial_list_pointer = (int *) list_ptr;
 for (i = 0; i < partial_list_size; i++)
 if (partial_list_pointer[i] < my_min)
 my_min = partial_list_pointer[i];
 /* lock the mutex associated with minimum_value and
 update the variable as required */
 pthread_mutex_lock(&minimum_value_lock);
 if (my_min < minimum_value)
 minimum_value = my_min;
 /* and unlock the mutex */
 pthread_mutex_unlock(&minimum_value_lock);
 pthread_exit(0);
}

