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

  char *endptr;
  unsigned long i, j, k, acc;
  int mul, next;

  upper_limit = strtoul(argv[1], &endptr, 10);

  if (*endptr) {
    printf("The upper limit value is invalid.\n");
    exit(EXIT_FAILURE);
  } else if (errno == ERANGE) {
    printf("The upper limit value is out of range.\n");
    exit(EXIT_FAILURE);
  }

  sieve_size = (upper_limit - 1) / 2 + 1;
  sieve = malloc((sieve_size + 1) * sizeof(char));

  sieve[0] = COMPOSITE;

  for (i = 1; i < sieve_size; i++)
    sieve[i] = PRIME;

  primes = sieve_size;

  acc = 0;
  next = -1;
  for (i = 1, k = 3; k * k <= upper_limit; i++, k += 2) {
    acc += k;
    next++;

    if (next == 3) {
      next = 0;
      continue;
    }

    if (k == 5)
      mul = 2;
    else
      mul = 1;
    if (!sieve[i]) {
      for (j = i * i + acc; j < sieve_size; j += k * mul) {
        if (!sieve[j]) {
          if (k == 5 && mul == 1)
            mul = 2;
          else
            mul = 1;
          sieve[j] = COMPOSITE;
          primes--;
        }
      }
    }
  }

  printf("There are %lu numbers marked as prime\n", primes);

  free(sieve);
  return 0;
}
