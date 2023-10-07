/**
 * MPI5Comm14°. В главном процессе дано целое число N (> 1), не превосходящее
 * количества процессов K. Переслать число N во все процессы, после чего
 * определить декартову топологию для начальной части процессов в виде
 * двумерной решетки — матрицы размера N × K/N (символ «/» обозначает операцию
 * деления нацело, порядок нумерации процессов следует оставить прежним). Для
 * каждого процесса, включенного в декартову топологию, вывести его координаты
 * в этой топологии.
 * */

#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int N;

  if (rank == 0) {
    N = 3;
  }

  MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int dims[2] = {N, size / N};
  int periods[2] = {1, 1};
  int reorder = 0;

  MPI_Comm cart_comm;
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cart_comm);

  int coords[2];
  MPI_Cart_coords(cart_comm, rank, 2, coords);

  printf("Rank %2d | coords (%d, %d)\n", rank, coords[0], coords[1]);

  MPI_Finalize();
  return 0;
}
