// Matrix-vector multiplication using distribution of tasks to forked
// processes. The communication is done through a mapped memory buffer.
//
// NOTE: This only works on Unix systems currently.

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#define UTILS_IMPLEMENTATION
#include "utils.h"

#define NUM_CHILD_PROCS 2
#define CHILD_PID 0


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

    //--- Matrix-vector multiplication

    // Distribute chunks of rows
    uint64_t num_chunks = NUM_CHILD_PROCS;
    uint64_t chunk_size = n / num_chunks;

    double tik = now();

    for (size_t proc_index = 0; proc_index < num_chunks; proc_index++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed.\n");
            exit(EXIT_FAILURE);
        }

        if (pid == CHILD_PID) {
            size_t start = proc_index * chunk_size;
            size_t end =
                (proc_index == num_chunks - 1) ? n : start + chunk_size;

            for (size_t i = start; i < end; i++) {
                for (size_t j = 0; j < n; j++) {
                    res[i] += mat[i * n + j] * vec[j];
                }
            }

            exit(EXIT_SUCCESS);
        }
    }

    for (size_t proc = 0; proc < num_chunks; proc++) {
        wait(NULL);
    }

    //--- Post-process

    double elapsed = now() - tik;

    for (size_t i = 0; i < n; i++) {
        fprintf(stdout, "%lf\n", res[i]);
    }

    fprintf(stderr, "Elapsed time: %lf s\n", elapsed);

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
