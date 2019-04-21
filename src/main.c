#include <errno.h>
#include <mpi.h>
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
            //printf("marked %ld\n", j);
            marked++;
          }
        }
      }
    }
  }

  return marked;
}

void Check_for_error(int local_ok, char *fname, char *message, MPI_Comm comm) {
  int ok;
  int my_rank;

  MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
  if (!ok) {
    MPI_Comm_rank(comm, &my_rank);
    if (my_rank == 0) {
      fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname, message);
      fflush(stderr);
    }
    MPI_Finalize();
    exit(EXIT_FAILURE);
  }
}

void Read_n(int *n_p, int my_rank, int comm_sz, MPI_Comm comm) {
  int local_ok = 1;
  char *fname = "Read_n";

  if (my_rank == 0) {
    printf("What's the upper limit value?\n");
    scanf("%d", n_p);
  }
  MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
  if (*n_p <= 0)
    local_ok = 0;
  Check_for_error(local_ok, fname, "upper value should be > 0", comm);
}

void Allocate_vector(SV_BLK_T **local_s_pp, int local_s_sz, MPI_Comm comm) {
  int local_ok = 1;
  char *fname = "Allocate_vector";

  *local_s_pp = malloc((local_s_sz / SV_BLK_SZ + 1) * sizeof(SV_BLK_T));

  if (*local_s_pp == NULL)
    local_ok = 0;

  Check_for_error(local_ok, fname, "can't allocate local vector(s)", comm);
}

int main(void) {
  int n;
  int comm_sz, my_rank;

  MPI_Comm comm;

  MPI_Init(NULL, NULL);
  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  Read_n(&n, my_rank, comm_sz, comm);
  printf("Proc %d > n = %d\n", my_rank, n);

  if (n < 2) {
    printf("0\n");
  } else if (n < 3) {
    printf("1\n");
  } else if (n < 5) {
    printf("2\n");
  } else {
    int64_t sieve_size, primes_count;

    sieve_size = sieveSize(n);
    primes_count = sieve_size + 2;

    if (n < 25) {
      printf("%ld\n", primes_count);
    } else {
      int64_t i, start, end, sieve_base;

      SV_BLK_T *sieve = NULL;
      int64_t local_s_sz = sieve_size;

      Allocate_vector(&sieve, local_s_sz, comm);

      for (i = 0; i < sieve_size; i++)
        SV_CLR_BIT(sieve, i);

      sieve_base = sieveIndexOf(sqrtul(n));

      uint64_t last = !(comm_sz - (my_rank + 1));
      uint64_t quota = (sieve_size / comm_sz);
      uint64_t skip = quota * my_rank;
      uint64_t take = quota + (sieve_size % comm_sz) * last;

      uint64_t marked = 0;
      for (start = skip, end = SV_BLK_CT; start < skip + take + 1;
           start = end, end += SV_BLK_CT) {
        printf("rank %d marking --start: %ld --limit: %ld\n", my_rank, start, skip + take);
        marked += sieveMark(sieve, skip + take + 1, sieve_base, start, end);
      }

      printf("primes_count: %ld\n", primes_count);
      printf("marked: %ld\n", marked);

      free(sieve);
    }
  }

  MPI_Finalize();

  return 0;
}
