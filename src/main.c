#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SV_BLK_T char  // SIEVE BLOCK TYPE
#define SV_BLK_SZ 8    // SIEVE BLOCK SIZE
#define SV_BLK_SZ_LG 3 // LOG BASE 2 OF SIEVE BLOCK SIZE

#define MOD(a, b) (a & (b - 1))
#define SET_BIT(t, sz, lg_sz, A, i) (A[(i >> lg_sz)] |= ((t)1 << MOD(i, sz)))
#define CLR_BIT(t, sz, lg_sz, A, i) (A[(i >> lg_sz)] &= ~((t)1 << MOD(i, sz)))
#define TST_BIT(t, sz, lg_sz, A, i) (A[(i >> lg_sz)] & ((t)1 << MOD(i, sz)))

#define SV_SET_BIT(A, i) (SET_BIT(SV_BLK_T, SV_BLK_SZ, SV_BLK_SZ_LG, A, i))
#define SV_CLR_BIT(A, i) (CLR_BIT(SV_BLK_T, SV_BLK_SZ, SV_BLK_SZ_LG, A, i))
#define SV_TST_BIT(A, i) (TST_BIT(SV_BLK_T, SV_BLK_SZ, SV_BLK_SZ_LG, A, i))

uint64_t sqrtul(uint64_t x) {
  uint64_t i;

  if (x == 0 || x == 1)
    return x;

  i = 2;
  while (i * i <= x)
    i++;

  return i - 1;
}

uint64_t mark(SV_BLK_T *sieve, uint64_t sieve_size, uint64_t upper_limit,
              uint64_t start, uint64_t end) {
  int8_t right, sign;
  uint64_t i, j, k, n, aux;
  uint64_t marked;

  marked = 0;

  for (i = 0, k = 1, n = 5, right = 0; n * n <= upper_limit;
       i++, k += 1 * right, n += 2 + 2 * right, right = !right) {
    if (!SV_TST_BIT(sieve, i)) {
      j = ((k * n - k) * 2 - 1) * !right + ((k + n * k) * 2 - 1) * right;

      if (start > j)
        j += 2 * n * ((start - j) / (2 * n));

      for (; j <= end && j < sieve_size; j += n + 2 * sign * k, sign *= -1)
        if (j > start && j <= end && j < sieve_size)
          if (!SV_TST_BIT(sieve, j)) {
            SV_SET_BIT(sieve, j);
            marked++;
          }
    }
  }

  return marked;
}

int main(int argc, char *argv[]) {
  SV_BLK_T *sieve;
  char *endptr;
  int8_t right, sign;
  uint64_t upper_limit, sieve_size, blocks, start, end, primes_count;
  uint64_t i, j, k, n, aux;

  upper_limit = strtoul(argv[1], &endptr, 10);

  if (*endptr) {
    printf("The upper limit value is invalid.\n");
    exit(EXIT_FAILURE);
  } else if (errno == ERANGE) {
    printf("The upper limit value is out of range.\n");
    exit(EXIT_FAILURE);
  }

  sieve_size = (upper_limit + 1) / 6 * 2;

  if (sieve_size / 2 * 6 >= upper_limit)
    sieve_size--;

  sieve = malloc((sieve_size / SV_BLK_SZ + 1) * sizeof(SV_BLK_T));
  for (i = 0; i < sieve_size; i++)
    SV_CLR_BIT(sieve, i);

  blocks = sqrtul(upper_limit);
  primes_count = sieve_size + 2;

  for (start = 0, end = blocks; start < sieve_size;
       start += blocks, end += blocks) {
    // printf("Test data between indexes %lu and %lu\n", start, end);
    primes_count -= mark(sieve, sieve_size, upper_limit, start, end);
  }

  /*
    for (i = 0, n = 5, right = 0; i < sieve_size;
         i++, n += 2 + 2 * right, right = !right) {
      if (!SV_TST_BIT(sieve, i))
        printf("sieve[%lu] has prime %lu\n", i, n);
    }
    */

  printf("%lu primes\n", primes_count);

  free(sieve);
  return 0;
}
