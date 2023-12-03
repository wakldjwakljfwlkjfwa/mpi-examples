#include <stdio.h>

struct Array {
  int size;
  int *arr;
};

void print_array(int *arr, int size) {
  for (int i = 0; i < size; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
}

int partition(int *arr, int lo, int hi) {
  int p = arr[hi];
  int idx = lo - 1;

  for (int i = lo; i < hi; i++) {
    if (arr[i] <= p) {
      idx++;
      int tmp = arr[idx];
      arr[idx] = arr[i];
      arr[i] = tmp;
    }
  }

  int tmp = arr[idx + 1];
  arr[idx + 1] = arr[hi];
  arr[hi] = tmp;

  return idx + 1;
}

void quicksort(int *arr, int lo, int hi) {
  if (lo < hi) {
    int p = partition(arr, lo, hi);

    quicksort(arr, lo, p - 1);
    quicksort(arr, p + 1, hi);
  }
}

int main() {
  int arr[] = {6, -20, 15, 20, 125, 1, 0, 1, -11};
  int size = sizeof(arr) / sizeof(int);

  quicksort(arr, 0, size - 1);

  print_array(arr, size);

  return 0;
}
