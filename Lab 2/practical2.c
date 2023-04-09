#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "cond.c"


int pnum;  // number updated when producer runs.
int csum;  // sum computed using pnum when consumer runs.

// indicates the status of the buffer (pnum)
int produce = 1;
int consume = 0;

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;	// protecting pnum
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER; // protecting csum (just in case)

pthread_cond_t can_produce = PTHREAD_COND_INITIALIZER;
pthread_cond_t can_consume = PTHREAD_COND_INITIALIZER;

int (*pred)(int); // predicate indicating if pnum is to be consumed

int produceT() {

  // acquire the lock for the mutexe for pnum
  pthread_mutex_lock(&m1);

  // wait for the consumer to finish consuming
  while(produce == 0) {

    pthread_cond_wait(&can_produce, &m1);

  }

  // the buffer (of one) is now full
  produce = 0;

  scanf("%d",&pnum); // read a number from stdin

  // sigmal the concumer to consume the current pnum
  pthread_cond_signal(&can_consume);

  // let the consumer know that the buffer (of one) is full
  consume = 1;

  // realease the lock for pnum
  pthread_mutex_unlock(&m1);
  return pnum;
}

void *Produce(void *a) {
  int p;

  p=1;

  while (p) {

    printf("@P-READY\n");
    p = produceT();
    printf("@PRODUCED %d\n",p);

  }


  printf("@P-EXIT\n");
  pthread_exit(NULL);
}


int consumeT() {

  // acquire the locks for pnum and csum
  pthread_mutex_lock(&m1);
  pthread_mutex_lock(&m2);


  // wait until the buffer is full
  while(consume == 0) {
    pthread_cond_wait(&can_consume, &m1);
  }

  // there is only one item to consume
  consume = 0;

  if ( pred(pnum) ) { csum += pnum; }

  // buffer is now empty so producer can produce
  produce = 1;

  // signal the producer thread
  pthread_cond_signal(&can_produce);


  // release the locks for pnum and csum
  pthread_mutex_unlock(&m2);
  pthread_mutex_unlock(&m1);

  return pnum;
}

void *Consume(void *a) {
  int p;

  p=1;

  while (p) {

    printf("@C-READY\n");
    p = consumeT();
    printf("@CONSUMED %d\n",csum);

  }

  printf("@C-EXIT\n");
  pthread_exit(NULL);
}


int main (int argc, const char * argv[]) {
  // the current number predicate
  static pthread_t prod,cons;
	long rc;

  pred = &cond1;
  if (argc>1) {
    if      (!strncmp(argv[1],"2",10)) { pred = &cond2; }
    else if (!strncmp(argv[1],"3",10)) { pred = &cond3; }
  }


  pnum = 999;
  csum=0;
  srand(time(0));

  printf("@P-CREATE\n");
  rc = pthread_create(&prod,NULL,Produce,(void *)0);
  if (rc) {
    printf("@P-ERROR %ld\n",rc);
    exit(-1);
  }
  printf("@C-CREATE\n");
  rc = pthread_create(&cons,NULL,Consume,(void *)0);
  if (rc) {
    printf("@C-ERROR %ld\n",rc);
    exit(-1);
  }

  printf("@P-JOIN\n");
  pthread_join(prod, NULL);
  printf("@C-JOIN\n");
  pthread_join(cons, NULL);



  printf("@CSUM=%d.\n",csum);

  return 0;
}
