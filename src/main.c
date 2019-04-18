#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SetComposite(S, k) (S[(k >> 5)] |= (1 << (k & (32 - 1))))
#define SetPrime(S, k) (S[(k >> 5)] &= ~(1 << (k & (32 - 1))))
#define IsComposite(S, k) (S[(k >> 5)] & (1 << (k & (32 - 1))))

int main(int argc, char *argv[]) {
  int32_t *sieve;
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

  sieve = malloc((sieve_size / 32 + 1) * sizeof(int32_t));

  for (i = 0; i < sieve_size; i++)
    SetPrime(sieve, i);

  primes = sieve_size + 2;

  for (i = 0, k = 1, n = 5, right = 0; n * n <= upper_limit;
       i++, k += 1 * right, n += 2 + 2 * right, right = !right) {
    if (!IsComposite(sieve, i)) {
      if (!right) {
        j = (k * n - k) * 2 - 1;
        signal = -1;
      } else {
        j = (k + n * k) * 2 - 1;
        signal = 1;
      }
      for (; j < sieve_size; j += 2 * n) {
        if (!IsComposite(sieve, j)) {
          SetComposite(sieve, j);
          primes--;
        }
        aux = j + n + 2 * signal * k;
        if (aux < sieve_size) {
          if (!IsComposite(sieve, aux)) {
            SetComposite(sieve, aux);
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
