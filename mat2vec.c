#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int *mat_part = malloc(size * sizeof(int));
  for (int i = 0; i < size; i++) {
    mat_part[i] = i + size * rank;
  }
  int n = rank + 1;

  int *vec = malloc(size * sizeof(int));

  MPI_Allgather(&n, 1, MPI_INT, vec, 1, MPI_INT, MPI_COMM_WORLD);

  int result = 0;
  for (int i = 0; i < size; i++) {
    result += mat_part[i] * vec[i];
  }

  int *mat_result = malloc(size * sizeof(int));
  MPI_Allgather(&result, 1, MPI_INT, mat_result, 1, MPI_INT, MPI_COMM_WORLD);

  if (rank == 0) {
    printf("Result is: [");
    for (int i = 0; i < size; i++) {
      printf("%d ", mat_result[i]);
    }
    printf("]\n");
  }

  free(mat_result);
  free(vec);
  free(mat_part);

  MPI_Finalize();
  return EXIT_SUCCESS;
}
