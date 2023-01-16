#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *thread_scanner(void *arg) {
  atomic_llong num = 0, sum = 0;
  while(1) {
    if (EOF == scanf("%lld", &num)) {
      pthread_exit((void *)sum);
      return NULL;
    }
    sum += num;
  }
}

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    return 1;
  }
  int err = 0;
  int N = strtol(argv[1], NULL, 10);
  atomic_llong sum = 0;
  
  pthread_t *threads = (pthread_t *)calloc(N, sizeof(pthread_t));
  for (int i = 0; i < N; ++i) {
    if ((err = pthread_create(threads + i, NULL, thread_scanner, NULL))) {
      fprintf(stderr, "pthread_create: Error code - %d", err);
      exit(1);
    }
  }
  void* partial_sum = NULL;
  for (int i = 0; i < N; ++i) {
    if ((err = pthread_join(threads[i], &partial_sum))) {
      fprintf(stderr, "pthread_join: Error code - %d", err);
      exit(1);
    }
    sum += (atomic_llong)partial_sum;
  }

  printf("%lld\n", sum);

  free(threads);
  return 0;
}
