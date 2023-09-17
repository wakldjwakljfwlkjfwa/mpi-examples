/*
 * MPI1Proc4°. В процессах четного ранга (включая главный) ввести целое число и
 * вывести его удвоенное значение. В процессах нечетного ранга не выполнять
 * никаких действий.
 * */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank % 2 == 0) {
    srand(time(NULL) + rank);
    int value = rand() % 10000;
    int twice = value * 2;
    printf("Rank %d: value = %d, value * 2 = %d.\n", rank, value, twice);
  } else {
    printf("Rank %d: doing nothing.\n", rank);
  }

  MPI_Finalize();

  return 0;
}
