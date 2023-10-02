#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int world_rank, world_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  // Определить цвет (номер строки)
  int color = world_rank % 4;
  // Разбить коммуникатор
  MPI_Comm row_comm;
  MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &row_comm);
  int row_rank, row_size;
  MPI_Comm_rank(row_comm, &row_rank);
  MPI_Comm_size(row_comm, &row_size);

  char *message = malloc(200 * sizeof(char));
  sprintf(message, "WORLD RANK/SIZE: %d/%d \t ROW RANK/SIZE: %d/%d\n",
          world_rank, world_size, row_rank, row_size);

  if (world_rank == 0) {
    printf("%s", message);
    for (int i = 1; i < world_size; i++) {
      char recv[200];
      MPI_Status status;
      MPI_Recv(&recv, 200, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);
      printf("%s", recv);
    }
  } else {
    MPI_Send(message, 200, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
  }

  free(message);
  MPI_Comm_free(&row_comm);

  MPI_Finalize();
  return 0;
}
