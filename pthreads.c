#include <pthread.h>
#include <stdio.h>

void *thread_job(void *arg) {
  int *param = (int *)arg;

  printf("thread_job %d\n", *param);

  return NULL;
}

int main() {
  unsigned int thread_num = 4;
  pthread_t threads[thread_num];
  int thread_args[thread_num];

  pthread_attr_t thread_attr;

  pthread_attr_init(&thread_attr);

  pthread_attr_setstacksize(&thread_attr, 5 * 1024 * 1024);

  for (int i = 0; i < thread_num; i++) {
    thread_args[i] = i;
    pthread_create(&threads[i], NULL, thread_job, (void *)&thread_args[i]);
  }

  pthread_attr_destroy(&thread_attr);

  pthread_exit(NULL);

  return 0;
}
