/**
 * MPI4Type14°. В главном процессе даны два набора целых чисел: A размера 3K и
 * N размера K, где K — количество подчиненных процессов. Считая, что элементы
 * наборов нумеруются от 1, переслать и вывести в каждом подчиненном процессе
 * ранга R (R = 1, 2, …, K) NR элементов из набора A, начиная с элемента AR и
 * перебирая их через один (например, если N2 равно 3, то в процесс ранга 2
 * надо переслать элементы A2, A4, A6). Использовать для пересылки каждого
 * набора элементов по одному вызову функций MPI_Send, MPI_Probe и MPI_Recv,
 * причем функция MPI_Recv должна возвращать массив, содержащий только те
 * элементы, которые требуется вывести. Для этого в главном процессе определить
 * новый тип, содержащий единственный целочисленный элемент и дополнительный
 * конечный пустой промежуток, равный размеру элемента целого типа.
 * Использовать в функции MPI_Send исходный массив A с требуемым смещением,
 * указав в качестве второго параметра количество элементов, равное NR, а в
 * качестве третьего параметра — новый тип. В функции MPI_Recv использовать
 * целочисленный массив размера NR и тип MPI_INT. Для определения количества
 * NR переданных элементов использовать в подчиненных процессах функцию
 * MPI_Get_count.
 *
 * Указание. Для задания завершающего пустого промежутка при определении нового
 * типа в MPI-2 следует использовать функцию MPI_Type_create_resized (в данном
 * случае эту функцию надо применить к типу MPI_INT). В MPI-1 надо использовать
 * метку нулевого размера типа MPI_UB совместно с функцией MPI_Type_struct (в
 * стандарте MPI-2 тип MPI_UB объявлен устаревшим).
 * */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == 0) {
    int K = size - 1;
    int A[3 * K];
    int N[K];

    for (int i = 0; i < 3 * K; i++) {
      A[i] = i + 1;
    }
    for (int i = 0; i < K; i++) {
      N[i] = i + 1;
    }

    MPI_Aint extent;
    MPI_Aint lb;
    MPI_Type_get_extent(MPI_INT, &lb, &extent);

    MPI_Datatype send_type;
    MPI_Type_create_resized(MPI_INT, lb, extent * 2, &send_type);
    MPI_Type_commit(&send_type);

    for (int i = 1; i <= K; i++) {
      MPI_Send(&A[i - 1], N[i - 1], send_type, i, 0, MPI_COMM_WORLD);
    }

  } else {
    MPI_Status status;
    int count;

    MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_INT, &count);

    int *recv = malloc(sizeof(int) * count);

    MPI_Recv(recv, count, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    printf("Rank %d received %d elements: ", rank, count);
    for (int i = 0; i < count; i++) {
      printf("%d ", recv[i]);
    }
    printf("\n");
    free(recv);
  }

  MPI_Finalize();
  return 0;
}
