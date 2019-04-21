#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SV_BLK_T char     // SIEVE BLOCK TYPE
#define SV_BLK_SZ 8       // SIEVE BLOCK SIZE
#define SV_BLK_SZ_LG 3    // LOG BASE 2 OF SIEVE BLOCK SIZE
#define SV_BLK_CT 2000000 // SIEVE BLOCKS MARKED AT ONCE

#define MOD(a, b) (a & (b - 1))
#define SET_BIT(t, sz, lg_sz, A, i) (A[(i >> lg_sz)] |= ((t)1 << MOD(i, sz)))
#define CLR_BIT(t, sz, lg_sz, A, i) (A[(i >> lg_sz)] &= ~((t)1 << MOD(i, sz)))
#define TST_BIT(t, sz, lg_sz, A, i) (A[(i >> lg_sz)] & ((t)1 << MOD(i, sz)))

#define SV_SET_BIT(A, i) (SET_BIT(SV_BLK_T, SV_BLK_SZ, SV_BLK_SZ_LG, A, i))
#define SV_CLR_BIT(A, i) (CLR_BIT(SV_BLK_T, SV_BLK_SZ, SV_BLK_SZ_LG, A, i))
#define SV_TST_BIT(A, i) (TST_BIT(SV_BLK_T, SV_BLK_SZ, SV_BLK_SZ_LG, A, i))

int64_t sqrtul(int64_t x) {
  int64_t i;

  if (x == 0 || x == 1)
    return x;

  i = 2;
  while (i * i <= x)
    i++;

  return i - 1;
}

int64_t sieveSize(int64_t upper_limit) {
  int64_t sieve_size;
  int64_t k;

  k = (upper_limit + 1) / 6;
  sieve_size = k * 2;
  if (upper_limit <= k * 6)
    sieve_size--;

  return sieve_size;
}

int64_t sieveIndexOf(int64_t value) { return sieveSize(value) - 1; }

int64_t sieveMark(SV_BLK_T *sieve, int64_t sieve_size, int64_t sieve_base,
                  int64_t start, int64_t end) {
  int64_t marked;
  int64_t i, j, k, n;
  int8_t right, sign;

  marked = 0;
  for (i = 0, k = 1, n = 5, right = 0; i <= sieve_base;
       i++, k += 1 * right, n += 2 + 2 * right, right = !right) {
    if (!SV_TST_BIT(sieve, i)) {
      if (!right) {
        sign = -1;
        j = (k * n - k) * 2 - 1;
      } else {
        sign = 1;
        j = (k + n * k) * 2 - 1;
      }

      if (start > j)
        j += 2 * n * ((start - j) / (2 * n));

      for (; j <= end && j < sieve_size; j += n + 2 * sign * k, sign *= -1) {
        if (j > start) {
          if (!SV_TST_BIT(sieve, j)) {
            SV_SET_BIT(sieve, j);
            marked++;
          }
        }
      }
    }
  }

  return marked;
}

int main(int argc, char *argv[]) {
  SV_BLK_T *sieve;
  int64_t sieve_size, sieve_base;
  int64_t upper_limit;
  char *endptr;
  int64_t i, n, start, end, primes_count;
  int8_t right;

  upper_limit = strtol(argv[1], &endptr, 10);

  if (*endptr) {
    printf("The upper limit value is invalid.\n");
    exit(EXIT_FAILURE);
  } else if (upper_limit < 1 || errno == ERANGE) {
    printf("The upper limit value is out of range.\n");
    exit(EXIT_FAILURE);
  }

  if (upper_limit < 2) {
    printf("0\n");
    exit(EXIT_SUCCESS);
  } else if (upper_limit < 3) {
    printf("1\n");
    exit(EXIT_SUCCESS);
  } else if (upper_limit < 5) {
    printf("2\n");
    exit(EXIT_SUCCESS);
  }

  sieve_size = sieveSize(upper_limit);
  primes_count = sieve_size + 2;

  if (upper_limit < 25) {
    printf("%ld\n", primes_count);
    exit(EXIT_SUCCESS);
  }

  sieve = malloc((sieve_size / SV_BLK_SZ + 1) * sizeof(SV_BLK_T));
  for (i = 0; i < sieve_size; i++)
    SV_CLR_BIT(sieve, i);

  sieve_base = sieveIndexOf(sqrtul(upper_limit));

  for (start = 0, end = SV_BLK_CT; start < sieve_size;
       start = end, end += SV_BLK_CT) {
    primes_count -= sieveMark(sieve, sieve_size, sieve_base, start, end);
  }

  printf("%ld\n", primes_count);

  free(sieve);
  return 0;
}
