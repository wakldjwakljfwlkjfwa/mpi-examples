/*
 * MPI2Send16°. В каждом процессе дано целое число N, причем для одного
 * процесса значение N равно 1, а для остальных равно 0. В процессе с N = 1
 * дан также набор из K − 1 числа, где K — количество процессов. Переслать из
 * этого процесса по одному из чисел данного набора в остальные процессы,
 * перебирая ранги получателей в возрастающем порядке, и вывести в каждом из
 * них полученное число.
 * */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int N = rank == 0 ? 1 : 0;

  enum { TAG_RECEIVED, TAG_SEND };

  switch (N) {
  case 1: {
    const int K = size - 1;
    int sequence[K];
    srand(time(NULL));
    printf("Sequence:");
    for (int i = 0; i < K; i++) {
      sequence[i] = rand() % 10000;
      printf(" %d", sequence[i]);
    }
    printf("\n");

    for (int i = 1; i < size; i++) {
      MPI_Send(&sequence[i - 1], 1, MPI_INT, i, TAG_SEND, MPI_COMM_WORLD);
      int buf;
      MPI_Recv(&buf, 1, MPI_INT, i, TAG_RECEIVED, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
    }
    break;
  }
  case 0: {
    int buf;
    MPI_Recv(&buf, 1, MPI_INT, 0, TAG_SEND, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("[Process %d] received value %d.\n", rank, buf);
    MPI_Send(&rank, 1, MPI_INT, 0, TAG_RECEIVED, MPI_COMM_WORLD);
    break;
  }
  }

  MPI_Finalize();
  return 0;
}
