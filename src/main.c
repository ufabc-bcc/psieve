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

void Read_n(int64_t *n_p, int argc, char **argv, int my_rank, int comm_sz,
            MPI_Comm comm) {
  int local_ok = 1;
  char *endptr;
  char *fname = "Read_n";

  if (my_rank == 0) {
    *n_p = strtoul(argv[1], &endptr, 10);

    if (*endptr) {
      *n_p = -1;
    } else if (*n_p <= 0 || errno == ERANGE) {
      *n_p = -1;
    }
  }
  MPI_Bcast(n_p, 1, MPI_INT64_T, 0, comm);
  if (*n_p <= 0)
    local_ok = 0;
  Check_for_error(local_ok, fname, "upper value should be > 0", comm);
}

void Allocate_vector(SV_BLK_T **local_s_pp, uint64_t local_s_sz,
                     MPI_Comm comm) {
  int local_ok = 1;
  char *fname = "Allocate_vector";

  *local_s_pp = malloc((local_s_sz / SV_BLK_SZ + 1) * sizeof(SV_BLK_T));

  if (*local_s_pp == NULL)
    local_ok = 0;

  Check_for_error(local_ok, fname, "can't allocate local vector(s)", comm);
}

int main(int argc, char *argv[]) {
  int64_t n;
  int comm_sz, my_rank;

  MPI_Comm comm;

  MPI_Init(NULL, NULL);
  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  Read_n(&n, argc, argv, my_rank, comm_sz, comm);

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

      Allocate_vector(&sieve, sieve_size, comm);

      for (i = 0; i < sieve_size; i++)
        SV_CLR_BIT(sieve, i);

      sieve_base = sieveIndexOf(sqrtul(n));

      int64_t my_start = SV_BLK_CT * my_rank;
      int64_t skip = SV_BLK_CT * comm_sz;

      int64_t marked = 0;
      for (start = my_start, end = my_start + SV_BLK_CT; start < sieve_size;
           start += skip, end += skip) {
        marked += sieveMark(sieve, sieve_size, sieve_base, start, end);
      }

      int64_t global_sum;
      MPI_Reduce(&marked, &global_sum, 1, MPI_UINT64_T, MPI_SUM, 0,
                 MPI_COMM_WORLD);

      if (my_rank == 0) {
        printf("primes_count: %ld\n", primes_count - global_sum);
      }

      free(sieve);
    }
  }

  MPI_Finalize();

  return 0;
}
