#include "mpi.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int buf;
  const int TAG = 5;

  if (rank == 0) {
    MPI_Send(&rank, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);
  }

  int from = (rank + size - 1) % size;
  int to = (rank + 1) % size;
  while (1) {
    MPI_Recv(&buf, 1, MPI_INT, from, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Process [%d], received from [%d]\n", rank, buf);
    sleep(1);
    MPI_Send(&rank, 1, MPI_INT, to, TAG, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
