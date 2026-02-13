#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define UTILS_IMPLEMENTATION
#include "utils.h"

// ---<>---

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

    double* mat = alloc_private_memory(n*n*sizeof(*mat), PROT_READ|PROT_WRITE);
    double* vec = alloc_private_memory(n*sizeof(*mat), PROT_READ|PROT_WRITE);
    double* res = alloc_private_memory(n*sizeof(*mat), PROT_READ|PROT_WRITE);

    //--- Initialize matrices and vectors

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            mat[i * n + j] = i + j;
        }

        vec[i] = i;
        res[i] = 0.0;
    }

    //--- Do the calculation (serial)

    const double tik = now();

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            res[i] += mat[i * n + j] * vec[j];
        }
    }

    const double elapsed = now() - tik;

    //--- Post-process

    print_vec(res, n);
    fprintf(stderr, "Elapsed time: %lf\n", elapsed);

    //--- Release resources

    dealloc_mapped_memory(mat, n*n*sizeof(*mat));
    dealloc_mapped_memory(vec, n*sizeof(*mat));
    dealloc_mapped_memory(res, n*sizeof(*mat));

    return 0;
}
