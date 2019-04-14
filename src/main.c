#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define COMPOSITE 1

int main(int argc, char *argv[]) {
  char *sieve;
  unsigned long upper_limit;
  unsigned long primes;

  char *endptr;
  unsigned long i, j;
  char first;

  upper_limit = strtoul(argv[1], &endptr, 10);

  if (*endptr) {
    printf("The upper limit value is invalid.\n");
    exit(EXIT_FAILURE);
  } else if (errno == ERANGE) {
    printf("The upper limit value is out of range.\n");
    exit(EXIT_FAILURE);
  }

  printf("Create a list of consecutive integers from 2 to %lu\n", upper_limit);
  sieve = calloc(upper_limit + 1, sizeof(char));
  sieve[0] = COMPOSITE;
  sieve[1] = COMPOSITE;

  primes = upper_limit - 1;
  printf("There are %lu numbers in this list\n", primes);

  for (i = 2; i * i <= upper_limit; i++) {
    if (!sieve[i]) {
      printf("Number %lu is marked as prime\n", i);
      first = 1;

      for (j = i * i; j <= upper_limit; j += i) {
        if (!sieve[j]) {
          if (first) {
            printf("Mark %lu", j);
            first = 0;
          } else {
            printf(", %lu", j);
          }

          sieve[j] = COMPOSITE;
          primes--;
        }
      }

      if (!first) {
        printf(" as composite\n");
      }
    }
  }

  printf("There are %lu numbers marked as prime\n", primes);

  first = 1;
  for (i = 2; i <= upper_limit; i++) {
    if (!sieve[i]) {
      if (first) {
        printf("%lu", i);
        first = 0;
      } else {
        printf(", %lu", i);
      }
    }
  }

  printf(" are the prime numbers between 2 and %lu\n", upper_limit);

  free(sieve);
  return 0;
}
