// MP3. Напишите OpenMP-программу, которая вычисляет число 𝜋 с точностью до N
// знаков после запятой. Используйте следующую формулу: Распределите работу по
// нитям с помощью OpenMP-директивы for.

#include <omp.h>
#include <stdio.h>

int main() {
  const long long N = 100000000;

  double pi = 0;

#pragma omp parallel for reduction(+ : pi)
  for (long long i = 0; i < N; i++) {
    double x = (i + 0.5) / N;
    pi += 4.0 / (1 + x * x);
  }

  pi /= N;
  printf("Pi: %.30f\n", pi);

  return 0;
}
