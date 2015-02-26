#include<stdio.h>
#include<pthread.h>
#include<math.h>
#define MIN_INT 0
typedef struct {
int readers;
int writer;
pthread_cond_t readers_proceed;
pthread_cond_t writer_proceed;
int pending_writers;
pthread_mutex_t read_write_lock;
} mylib_rwlock_t;
 mylib_rwlock_t read_write_lock;
 void *find_min_rw(void *list_ptr);
 void mylib_rwlock_init (mylib_rwlock_t *l);
 void mylib_rwlock_rlock(mylib_rwlock_t *l);
 void mylib_rwlock_wlock(mylib_rwlock_t *l);
 void mylib_rwlock_unlock(mylib_rwlock_t *l);

 int minimum_value, partial_list_size,list_elements_size;
 int j=0;
main() {
 /* declare and initialize data structures and list */
int i,k; 
minimum_value = MIN_INT;
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
pthread_create(&thread[k],NULL,(void *)&find_min_rw,(void *)&list_elements[j]);
j=j+partial_list_size;
}
for(i=0;i< pthread_count;i++)
{
pthread_join(thread[i],NULL);
}
printf("The minimum value in the list : %d",minimum_value);
} 
void *find_min_rw(void *list_ptr) {
int *partial_list_pointer, my_min, i;
my_min = MIN_INT;
partial_list_pointer = (int *) list_ptr;
for (i = 0; i < partial_list_size; i++)
if (partial_list_pointer[i] < my_min)
	my_min = partial_list_pointer[i];
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
 pthread_exit(0);
 }
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
 l -> writer ++;
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
