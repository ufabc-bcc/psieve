#include <errno.h>
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

int main(int argc, char *argv[]) {
  SV_BLK_T *sieve;
  char *endptr;
  int right, sign;
  unsigned long upper_limit, sieve_size, primes_count;
  unsigned long i, j, k, n, aux;
  unsigned long s, m;

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

  sieve = malloc((sieve_size / SV_BLK_SZ + 1) * sizeof(SV_BLK_T));

  for (i = 0; i < sieve_size; i++)
    SV_CLR_BIT(sieve, i);

  primes_count = sieve_size + 2;

  for (s = 0; s <= sieve_size; s += 100000000) {
    for (i = 0, k = 1, n = 5, right = 0; n * n <= upper_limit;
         i++, k += 1 * right, n += 2 + 2 * right, right = !right) {
      if (!SV_TST_BIT(sieve, i)) {
        if (!right) {
          j = (k * n - k) * 2 - 1;
          sign = -1;
        } else {
          j = (k + n * k) * 2 - 1;
          sign = 1;
        }
        m = 0;
        if (s > j) {
          m = (s - j) / (2 * n);
        }
        j += 2 * n * m;
        // printf("for n = %lu starting j is %lu (s = %lu, m = %lu)\n", n, j, s, m);
        for (; j <= s + 100000000 && j < sieve_size; j += 2 * n) {
          if (!SV_TST_BIT(sieve, j)) {
            SV_SET_BIT(sieve, j);
            primes_count--;
          }
          aux = j + n + 2 * sign * k;
          if (aux < sieve_size) {
            if (!SV_TST_BIT(sieve, aux)) {
              SV_SET_BIT(sieve, aux);
              primes_count--;
            }
          }
        }
      }
    }
  }

/*
  for (i = 0, n = 5, right = 0; i < sieve_size;
       i++, n += 2 + 2 * right, right = !right)
    if (!SV_TST_BIT(sieve, i))
      printf("S[%lu] is %lu\n", i, n);*/

  printf("%lu primes\n", primes_count);

  free(sieve);
  return 0;
}
