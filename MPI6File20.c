/*
MPI6File20°. В главном процессе дано имя файла. Кроме того, в каждом процессе
дан набор из R + 1 вещественного числа, где R — ранг процесса (0, 1, …).
Создать новый файл вещественных чисел с указанным именем. Используя
единственный вызов коллективной функции MPI_File_write_all (и не применяя
функцию MPI_File_seek), записать в файл все данные числа в порядке, обратном
их следованию в исходном наборе: вначале записываются (в обратном порядке)
все числа из процесса с наибольшим рангом, затем все числа из процесса с
предыдущим рангом, и т. д. Для этого предварительно с помощью функции
MPI_File_set_view определить новый вид данных с базовым типом MPI_DOUBLE,
таким же файловым типом и подходящим смещением (своим для каждого процесса).
*/

#include "mpi.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  char filename[50];
  if (rank == 0) {
    strcpy(filename, "output.txt");
  }

  MPI_Bcast(filename, 50, MPI_CHAR, 0, MPI_COMM_WORLD);

  MPI_File file;
  MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY,
                MPI_INFO_NULL, &file);

  int data_size = rank + 1;
  double *data = malloc(sizeof(double) * data_size);
  for (int i = 0; i < data_size; i++) {
    data[i] = i;
  }

  int offset =
      (size * (size + 1) - (rank + 2) * (rank + 1)) / 2 * sizeof(double);
  MPI_File_set_view(file, offset, MPI_DOUBLE, MPI_DOUBLE, "native",
                    MPI_INFO_NULL);
  MPI_File_write_all(file, data, data_size, MPI_DOUBLE, MPI_STATUS_IGNORE);

  MPI_File_close(&file);

  MPI_Finalize();
  return 0;
}
