// Matrix-vector multiplication using distribution of tasks to forked
// processes. The communication is done through a mapped memory buffer.
//
// NOTE: This only works on Unix systems currently.

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>

#define UTILS_IMPLEMENTATION
#include "utils.h"

int main(int argc, char* argv[])
{
    //--- Handle commandline arguments

    if (argc < 2) {
        fprintf(stderr,
                "Usage: matvec_ipc <n>\n"
                "<n>: size of the system; 10 <= n <= 10000.\n");
        exit(EXIT_FAILURE);
    }
    int64_t n_input = atol(argv[1]);
    assert(n_input >= 10 && n_input <= 10000);
    uint64_t n = (uint64_t)n_input;

    //--- Allocate memory

    double* mat = mmap(NULL, n*n*sizeof(*mat), PROT_READ | PROT_WRITE,
                       MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    assert(mat != MAP_FAILED);
    double* vec = mmap(NULL, n*sizeof(*mat), PROT_READ | PROT_WRITE,
                       MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    assert(vec != MAP_FAILED);
    double* res = mmap(NULL, n*sizeof(*mat), PROT_READ | PROT_WRITE,
                       MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    assert(res != MAP_FAILED);

    //--- Initialize matrices and vectors

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            mat[i * n + j] = i + j;
        }

        vec[i] = i;
        res[i] = 0;
    }

    //--- Do the calculation (serial)

    const double tik = now();

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            res[i] += mat[i * n + j] * vec[j];
        }
    }

    //--- Post-process

    const double elapsed = now() - tik;

    print_vec(res, n);
    fprintf(stderr, "Elapsed time: %lf\n", elapsed);

    //--- Release resources

    int err = 0;
    err = munmap(mat, n*n*sizeof(*mat));
    assert(err >= 0);
    err = munmap(vec, n*sizeof(*mat));
    assert(err >= 0);
    err = munmap(res, n*sizeof(*mat));
    assert(err >= 0);

    return 0;
}
