/**
 * MPI3Coll10°.
 * В главном процессе дан набор из K + 2 чисел, где K — количество процессов.
 * Используя функцию MPI_Scatterv, переслать в каждый процесс три числа из
 * данного набора; при этом в процесс ранга R должны быть пересланы числа с
 * номерами от R + 1 до R + 3 (в процесс 0 — первые три числа,
 * в процесс 1 — числа со второго по четвертое, и т. д.).
 * В каждом процессе вывести полученные числа.
 * */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

void printArr(int *arr, int size, int rank) {
  printf("rank: %d | ", rank);
  for (int i = 0; i < size; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const int SEND_SIZE = 3;
  int *recv = (int *)malloc(SEND_SIZE * sizeof(int));

  int buf_size = size + 2;
  int *buf = NULL;
  if (rank == 0) {
    buf = (int *)malloc(buf_size * sizeof(int));
    for (int i = 0; i < buf_size; i++) {
      buf[i] = i + 1;
    }
  }

  int *displs = (int *)malloc(size * sizeof(int));
  for (int i = 0; i < size; i++) {
    displs[i] = i;
  }

  int *sendcounts = (int *)malloc(size * sizeof(int));
  for (int i = 0; i < size; i++) {
    sendcounts[i] = SEND_SIZE;
  }

  MPI_Scatterv(buf, sendcounts, displs, MPI_INT, recv, SEND_SIZE, MPI_INT, 0,
               MPI_COMM_WORLD);

  printArr(recv, SEND_SIZE, rank);

  free(sendcounts);
  free(displs);
  free(buf);
  free(recv);

  MPI_Finalize();
  return EXIT_SUCCESS;
}
