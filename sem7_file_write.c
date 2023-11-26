#include "mpi.h"
#include <stdio.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_File file;

  MPI_File_open(MPI_COMM_WORLD, "output.txt", MPI_MODE_CREATE | MPI_MODE_WRONLY,
                MPI_INFO_NULL, &file);

  MPI_File_set_view(file, rank * sizeof(int), MPI_INT, MPI_INT, "native", MPI_INFO_NULL);

  MPI_File_write(file, &rank, 1, MPI_INT, MPI_STATUS_IGNORE);

  MPI_File_close(&file);

  MPI_Finalize();
  return 0;
}
