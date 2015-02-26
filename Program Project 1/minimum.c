#include<stdio.h>
#include<pthread.h>
#include<math.h>
#define MIN_INT 0

 void *find_min(void *list_ptr);
 pthread_mutex_t minimum_value_lock;
 int minimum_value, partial_list_size,list_elements_size;
 int j=0;
main() {
 /* declare and initialize data structures and list */
int i,k; 
minimum_value = MIN_INT;
pthread_mutex_init(&minimum_value_lock, NULL);
int pthread_count;
printf("Enter the values for list size and thread count:");
scanf("%d %d",&list_elements_size,&pthread_count);
pthread_t thread[pthread_count];
static int list_elements[100000000];
for(i=0;i<list_elements_size;i++)
{
list_elements[i]=rand_r()%100000000 + (-99999999);
}
/*printf("List of Elements:");
for(i=0;i<list_elements_size;i++)
{
printf("%d\n",list_elements[i]);
}*/
partial_list_size=list_elements_size/pthread_count;
for(k=0;k<pthread_count;k++)
{
pthread_create(&thread[k],NULL,(void *)&find_min,(void *)&list_elements[j]);
j=j+partial_list_size;
}
for(i=0;i< pthread_count;i++)
{
pthread_join(thread[i],NULL);
}
printf("The minimum value in the list : %d",minimum_value);
} 
/* initialize lists, list_ptr, and partial_list_size */
 /* create and join threads here */

 void *find_min(void *list_ptr) {
 j=1;
 int *partial_list_pointer, my_min,i;
 my_min = MIN_INT;
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
