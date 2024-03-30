/* threads-example.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include<pthread.h>

#define CHILDREN 8   /* based on a true story.... */

/* function executed by each thread */
void * whattodo( void * arg );

int main()
{
  pthread_t tid[CHILDREN];   /* keep track of the thread IDs (tids) */

  int i, rc;

#if 0
  int t;  /* statically allocates 4 bytes on the runtime stack */
#endif

  /* create all the children */
  for ( i = 0 ; i < CHILDREN ; i++ )
  {
    int *t = malloc(sizeof(int));
    *t = 2 + i * 2;   /* 2, 4, 6, 8, ... */

    printf( "MAIN: Creating a thread to sleep for %d seconds\n", *t );

    rc = pthread_create(&tid[i], NULL, whattodo, t);
    if (rc != 0)
    {
      fprintf(stderr, "pthread_create() failed: %d\n",rc);
      break;
    }
  }

  /* Wait for Child threads to complete. */
  for (int ctr=0; ctr < CHILDREN; ctr++)
  {
    unsigned int *x;
    rc = pthread_join(tid[ctr], (void **)&x);
    if (rc != 0)
    {
      fprintf(stderr, "pthread_join() failed: %d\n",rc);
      break;
    }
    else
    {
      printf("MAIN: Joined %ld (x: 0x%x)\n", tid[ctr], *x);
    }
    free(x);
  }

#if 0
  sleep( 20 );
#endif

  return EXIT_SUCCESS;  /* exit the main thread... */

  /* when the main thread (or any child thread) exits/terminates its process,
     all other threads are terminated immediately */
}

void * whattodo( void * arg )
{
  int t = *(int *)arg;
  free(arg);

  printf( "THREAD %ld: I'm going to nap for %d seconds\n", pthread_self(), t );
  sleep( t );
  printf( "THREAD %ld: I'm awake\n", pthread_self() );

  unsigned int *y = malloc(sizeof(*y));
  *y = pthread_self();
  pthread_exit(y);  /* child thread ends here */
}
