#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int send_size = 4;
  int *send_data = malloc(send_size * sizeof(int));

  for (int i = 0; i < send_size; i++) {
    send_data[i] = rank * send_size + i;
  }

  int *recv_data = malloc(send_size * size * sizeof(int));

  MPI_Allgather(send_data, send_size, MPI_INT, recv_data, send_size, MPI_INT,
                MPI_COMM_WORLD);

  printf("Process %d:\n", rank);
  for (int i = 0; i < send_size * size; i++) {
    printf("%d\t", recv_data[i]);
    if ((i + 1) % send_size == 0) {
      printf("\n");
    }
  }
  printf("\n");

  free(recv_data);
  free(send_data);

  MPI_Finalize();
  return EXIT_SUCCESS;
}
