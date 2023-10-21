/*
MPI7Win18°. Количество процессов K — четное число. В каждом процессе четного
ранга (0, 2, …, K − 2) дано целое число A. Во всех процессах нечетного ранга (1,
3, …, K − 1) определить окно доступа из одного целого числа и, используя функцию
MPI_Put в каждом процессе четного ранга 2N, переслать число A в процесс ранга 2N
+ 1 и вывести его в этом процессе. Для синхронизации использовать функции
MPI_Win_start и MPI_Win_complete в процессах четного ранга и функции
MPI_Win_post и MPI_Win_wait в процессах нечетного ранга; для создания группы
процессов, указываемой в качестве первого параметра функций MPI_Win_start и
MPI_Win_post, использовать функцию MPI_Group_incl, применив ее к группе,
полученной из коммуникатора MPI_COMM_WORLD (с помощью функции MPI_Comm_group).

Примечание. В отличие от коллективной функции MPI_Win_fence, использовавшейся в
предыдущих заданиях данной группы, синхронизирующие функции, используемые в этом
и последующих заданиях, являются локальными и, кроме того, позволяют явным
образом определить группы инициирующих (origin) и целевых (target) процессов при
односторонних коммуникациях.
*/

#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Group world_group, even_group, odd_group;
  MPI_Comm_group(MPI_COMM_WORLD, &world_group);

  int even_ranks[size / 2];
  int odd_ranks[size / 2];

  for (int i = 0; i < size / 2; i++) {
    even_ranks[i] = 2 * i;
    odd_ranks[i] = 2 * i + 1;
  }

  MPI_Group_incl(world_group, size / 2, even_ranks, &even_group);
  MPI_Group_incl(world_group, size / 2, odd_ranks, &odd_group);

  int A;
  MPI_Win win;
  MPI_Win_create(&A, sizeof(A), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD,
                 &win);

  if (rank % 2 == 0) {
    A = rank * 10;
    MPI_Win_start(odd_group, 0, win);
    MPI_Put(&A, 1, MPI_INT, rank + 1, 0, 1, MPI_INT, win);
    MPI_Win_complete(win);
  } else {
    MPI_Win_post(even_group, 0, win);
    MPI_Win_wait(win);
    printf("Rank %d | value %d received from Rank %d\n", rank, A, rank - 1);
  }

  MPI_Win_free(&win);

  MPI_Group_free(&even_group);
  MPI_Group_free(&odd_group);
  MPI_Group_free(&world_group);

  MPI_Finalize();

  return 0;
}
