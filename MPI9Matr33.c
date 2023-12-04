// MPI9Matr33°. В каждом процессе даны числа M0 и P0, а также матрица A размера
// M0 × P0. Известно, что количество процессов K является полным квадратом: K =
// K0·K0. Прочесть в каждом процессе матрицу A в одномерный массив размера M0·P0
// и определить на множестве исходных процессов коммуникатор MPI_COMM_GRID,
// задающий декартову топологию двумерной квадратной циклической решетки порядка
// K0 (исходный порядок нумерации процессов сохраняется).
//
// При определении коммуникатора MPI_COMM_GRID использовать функцию
// MPI_Cart_create, оформив это определение в виде функции
// Matr4CreateCommGrid(comm) с выходным параметром comm типа MPI_Comm. Вывести в
// каждом процессе координаты (I0, J0) этого процесса в созданной топологии,
// используя функцию MPI_Cart_coords.
//
// На основе коммуникатора MPI_COMM_GRID создать набор коммуникаторов
// MPI_COMM_ROW, связанных со строками исходной двумерной решетки. Для
// определения коммуникаторов MPI_COMM_ROW использовать функцию MPI_Cart_sub,
// оформив это определение в виде вспомогательной функции
// Matr4CreateCommRow(grid, row) с входным параметром grid (коммуникатором,
// связанным с исходной двумерной решеткой) и выходным параметром row (оба
// параметра типа MPI_Comm). Вывести в каждом процессе его ранг R0 для
// коммуникатора MPI_COMM_ROW (этот ранг должен совпадать со значением J0).
//
// Кроме того, для каждой строки I0 полученной решетки осуществить пересылку
// матрицы A из столбца I0 во все процессы этой же строки, используя
// коллективную функцию MPI_Bcast для коммуникатора MPI_COMM_ROW и сохранив
// результат во вспомогательной матрице T того же размера, что и матрица A
// (перед пересылкой необходимо скопировать в матрицу T рассылающего процесса
// элементы пересылаемой матрицы A). Вывести в каждом процессе полученную
// матрицу T.

#include "mpi.h"
#include <stdio.h>

#define DIM_SIZE 3

void array_copy(int *to, int *from, int size) {
  for (int i = 0; i < size; i++) {
    to[i] = from[i];
  }
}

void array_print(int *arr, int size) {
  for (int i = 0; i < size; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
}

void Matr4CreateCommGrid(MPI_Comm *comm) {
  int dims[] = {DIM_SIZE, DIM_SIZE};
  int periods[] = {0, 0};
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, comm);
}

void Matr4CreateCommRow(MPI_Comm *grid, MPI_Comm *row) {
  int remain_dims[] = {1, 0};
  MPI_Cart_sub(*grid, remain_dims, row);
}

int main(int argc, char **argv) {

  MPI_Init(&argc, &argv);

  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  const int M = 4, P = 5;
  const int SIZE = M * P;
  int A[SIZE];

  for (int i = 0; i < SIZE; i++) {
    A[i] = i * (rank + 1);
  }

  MPI_Comm grid;
  Matr4CreateCommGrid(&grid);

  int coords[2];
  MPI_Cart_coords(grid, rank, 2, coords);

  MPI_Comm row;
  Matr4CreateCommRow(&grid, &row);

  int row_rank;
  MPI_Comm_rank(row, &row_rank);

  printf("Process %d coords: (%d, %d) | Row rank %d\n", rank, coords[0],
         coords[1], row_rank);

  MPI_Barrier(MPI_COMM_WORLD);

  int T[SIZE];
  array_copy(T, A, SIZE);

  MPI_Bcast(T, SIZE, MPI_INT, 0, row);

  printf("process %d | ", rank);
  array_print(T, SIZE);

  MPI_Comm_free(&row);
  MPI_Comm_free(&grid);
  MPI_Finalize();

  return 0;
}
