#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int format_message(char *message, int world_rank, int local_rank, int color) {
  return sprintf(message, "\033[%dm%d/%d\033[0m", 31 + (color % 7), world_rank,
                 local_rank);
}

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int world_rank, world_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  int number_of_comms = 4;
  // Определить цвет (номер строки)
  int color = world_rank % number_of_comms;
  // Разбить коммуникатор
  MPI_Comm row_comm;
  MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &row_comm);
  int row_rank, row_size;
  MPI_Comm_rank(row_comm, &row_rank);
  MPI_Comm_size(row_comm, &row_size);

  char *message = malloc(200 * sizeof(char));
  format_message(message, world_rank, row_rank, color);

  if (world_rank == 0) {
    printf("%s\t", message);
    for (int i = 1; i < world_size; i++) {
      char recv[200];
      MPI_Status status;
      MPI_Recv(&recv, 200, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);
      printf("%s\t", recv);
      if (i % number_of_comms == number_of_comms - 1) {
        printf("\n");
      }
    }
    printf("\n");
  } else {
    MPI_Send(message, 200, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
  }

  free(message);
  MPI_Comm_free(&row_comm);

  MPI_Finalize();
  return 0;
}
