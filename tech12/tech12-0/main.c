#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

struct arg_s {
  int i;
  int N;
  int k;
  double *nums;
  pthread_mutex_t *mtx;
};

void *thread_worker(void *arguments) {
  int err = 0;
  struct arg_s *arg = (struct arg_s *) arguments;
  pthread_mutex_t *mtx = arg->mtx;
  if ((err = pthread_mutex_lock(mtx))) {
    fprintf(stderr, "pthread_mutex_lock: Error code - %d", err);
    exit(1);
  }
  
  int i = arg->i;
  int N = arg->N;
  int k = arg->k;
  double *nums = arg->nums;

  for (int j = 0; j < N; j++) {
    nums[i] += 1.0;
    nums[(i + 1) % k] += 1.01;
    if (i > 0) 
      nums[i - 1] += 0.99;
    else 
      nums[k - 1] += 0.99;
  }
  if ((err = pthread_mutex_unlock(mtx))) {
    fprintf(stderr, "pthread_mutex_unlock: Error code - %d", err);
    exit(1);
  }
}

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    return 1;
  }
  int err = 0;

  int N = strtol(argv[1], NULL, 10);
  int k = strtol(argv[2], NULL, 10);

  pthread_mutex_t mtx;
  pthread_t *threads = (pthread_t *)calloc(k, sizeof(pthread_t));
  double *nums = (double *)calloc(k, sizeof(double));
  if ((err = pthread_mutex_init(&mtx, NULL))) {
    fprintf(stderr, "pthread_mutex_init: Error code - %d", err);
    exit(1);
  }

  struct arg_s *args = (struct arg_s *)calloc(k, sizeof(struct arg_s));
  for (int i = 0; i < k; i++) {
    args[i].i = i;
    args[i].N = N;
    args[i].k = k;
    args[i].nums = nums;
    args[i].mtx = &mtx;
    if ((err = pthread_create(threads + i, NULL, thread_worker, (void *)(args + i)))) {
      fprintf(stderr, "pthread_create: Error code - %d", err);
      exit(1);
    }
  }

  for (int i = 0; i < k; i++) {
    if ((err = pthread_join(threads[i], NULL))) {
      fprintf(stderr, "pthread_join: Error code - %d", err);
      exit(1);
    }
  }

  if ((err = pthread_mutex_destroy(&mtx))) {
      fprintf(stderr, "pthread_mutex_destroy: Error code - %d", err);
      exit(1);
    }

  for (int i = 0; i < k; i++) {
    printf("%.10g ", nums[i]);
  }

  printf("\n");

  free(threads);
  free(nums);

  return 0;

}