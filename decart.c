/**
 * Создать 3д декарт топологию и кажды процесс выводит ранг соседей(верхний,
 * нижний, левый, правый, передний, задний) и координаты каждого процесса
 * */

#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int size, rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int dims[3] = {3, 3, 3};
  int periods[3] = {1, 1, 1};
  int reorder = 1;

  MPI_Comm cart_comm;
  MPI_Cart_create(MPI_COMM_WORLD, 3, dims, periods, reorder, &cart_comm);

  int coords[3];
  MPI_Cart_coords(cart_comm, rank, 3, coords);

  int left, right, top, bottom, front, back;
  MPI_Cart_shift(cart_comm, 0, 1, &left, &right);
  MPI_Cart_shift(cart_comm, 1, 1, &top, &bottom);
  MPI_Cart_shift(cart_comm, 2, 1, &front, &back);

  printf("Rank %2d | coords: (%d, %d, %d) | l: %2d, r: %2d, top: %2d, bot: "
         "%2d, f: "
         "%2d, b: %2d\n",
         rank, coords[0], coords[1], coords[2], left, right, top, bottom, front,
         back);

  MPI_Finalize();
  return 0;
}
