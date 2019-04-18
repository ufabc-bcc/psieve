#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define COMPOSITE 1
#define PRIME 0

int main(int argc, char *argv[]) {
  char *sieve;
  char *endptr;
  int right, signal;
  unsigned long upper_limit, sieve_size, primes;
  unsigned long i, j, k, n, aux;

  upper_limit = strtoul(argv[1], &endptr, 10);

  if (*endptr) {
    printf("The upper limit value is invalid.\n");
    exit(EXIT_FAILURE);
  } else if (errno == ERANGE) {
    printf("The upper limit value is out of range.\n");
    exit(EXIT_FAILURE);
  }

  sieve_size = (upper_limit + 1) / 6 * 2;

  if (sieve_size / 2 * 6 > upper_limit)
    sieve_size--;

  sieve = malloc((sieve_size + 1) * sizeof(char));

  for (i = 0; i < sieve_size; i++)
    sieve[i] = PRIME;

  primes = sieve_size + 2;

  for (i = 0, k = 1, n = 5, right = 0; n * n <= upper_limit;
       i++, k += 1 * right, n += 2 + 2 * right, right = !right) {
    if (!sieve[i]) {
      if (!right) {
        j = (k * n - k) * 2 - 1;
        signal = -1;
      } else {
        j = (k + n * k) * 2 - 1;
        signal = 1;
      }
      for (; j < sieve_size; j += 2 * n) {
        if (!sieve[j]) {
          sieve[j] = COMPOSITE;
          primes--;
        }
        aux = j + n + 2 * signal * k;
        if (aux < sieve_size) {
          if (!sieve[aux]) {
            sieve[aux] = COMPOSITE;
            primes--;
          }
        }
      }
    }
  }

  printf("%lu primes\n", primes);

  free(sieve);
  return 0;
}
