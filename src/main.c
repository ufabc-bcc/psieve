#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define COMPOSITE 1
#define PRIME 0

int main(int argc, char *argv[]) {
  char *sieve;
  unsigned long sieve_size;
  unsigned long upper_limit;
  unsigned long primes;

  int right, s;
  unsigned long i, j, k, p, q, n, ios;

  char *endptr;

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

  // printf("Sieve has size %lu with %lu possible primes\n", sieve_size, primes);

  for (i = 0, k = 1, n = 5, right = 0; n * n <= upper_limit;
       i++, k += 1 * right, n += 2 + 2 * right, right = !right) {
    // printf("i = %lu, k = %lu, n = %lu, right = %d", i, k, n, right);
    if (!sieve[i]) {
      if (!right) {
        ios = (k * n - k) * 2 - 1;
        s = -1;
      }
      else {
        ios = (k + n * k) * 2 - 1;
        s = 1;
      }
      // printf(", ios = %lu", ios);
      // printf("\n");
      // printf("For %lu mark indexes: ", n);
      for (j = ios; j < sieve_size; j += 2 * n) {
        // printf("For n = %lu, with %lu primes, remove:\n", n, primes);
        p = j;
        q = j + n + 2 * s * k;
        if (!sieve[p]) {
          // printf("sieve[%lu]\n", p);
          sieve[p] = COMPOSITE;
          primes--;
        }
        if (q < sieve_size) {
          if (!sieve[q]) {
            // printf("sieve[%lu]\n", q);
            sieve[q] = COMPOSITE;
            primes--;
          }
        }
        
        // printf("\n");
      }
    }
    // printf("\n");
  }

  printf("%lu primes\n", primes);

  /*
  for (i = 0, n = 5, right = 0; i < sieve_size;
       i++, n += 2 + 2 * right, right = !right) {
    if (!sieve[i]) {
      printf("sieve[%lu] = %lu\n", i, n);
    }
  }
  */

  free(sieve);
  return 0;
}
