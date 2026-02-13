#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#define UTILS_IMPLEMENTATION
#include "utils.h"

#define NUM_THREADS 2

typedef struct _Chunk {
    size_t start_index;
    size_t end_index;
} Chunk;

typedef struct _Thread_Data {
    double* mat;
    double* vec;
    double* res;
    size_t  n;
    Chunk   chunk;
} Thread_Data;

static void* matvec_worker(void* data);

// --- <> ---

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr,
                "Usage: matvec_pthread <n>\n"
                "<n>: size of the system; 10 <= n <= 10000.\n");
        exit(EXIT_FAILURE);
    }
    int64_t n_input = atol(argv[1]);
    assert(n_input >= 10 && n_input <= 10000);
    uint64_t n = (uint64_t)n_input;

    //--- Allocate memory

    double* mat =
        alloc_private_memory(n * n * sizeof(*mat), PROT_READ | PROT_WRITE);
    double* vec =
        alloc_private_memory(n * sizeof(*vec), PROT_READ | PROT_WRITE);
    double* res =
        alloc_private_memory(n * sizeof(*res), PROT_READ | PROT_WRITE);

    //--- Initialize matrices, vectors, threads and thread data

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            mat[i * n + j] = i + j;
        }

        vec[i] = i;
        res[i] = 0;
    }

    // Divide rows into chunks
    uint64_t num_chunks = NUM_THREADS, num_threads = NUM_THREADS;
    uint64_t chunk_size = n / num_chunks;

    // Allocate threads and thread data
    pthread_t* threads =
        alloc_private_memory(num_threads * sizeof(*threads),
                             PROT_READ | PROT_WRITE);
    Thread_Data* thread_data =
        alloc_private_memory(num_threads * sizeof(*thread_data),
                             PROT_READ | PROT_WRITE);

    for (size_t th = 0; th < num_threads; th++) {
        thread_data[th] = (Thread_Data) {
            .mat = mat,
            .vec = vec,
            .res = res,
            .n = n,
            .chunk = {
                .start_index = th * chunk_size,
                .end_index =
                    (th == num_threads - 1) ? n : (th + 1) * chunk_size
            }
        };
    }

    #if 0
    print_mat(mat, n, n);
    putchar('\n');
    print_vec(vec, n);
    #endif

    //--- Dot matrix-vector multiplication

    const double tik = now();

    // Spawn threads
    for (size_t th = 0; th < NUM_THREADS; th++) {
        int err = pthread_create(&threads[th], NULL,
                                 matvec_worker, &thread_data[th]);
        assert(err == 0);
    }

    // Join threads
    for (size_t th = 0; th < NUM_THREADS; th++) {
        pthread_join(threads[th], NULL);
    }

    const double elapsed = now() - tik;

    //--- Post-processing

    for (size_t i = 0; i < n; i++) {
        fprintf(stdout, "%lf\n", res[i]);
    }

    fprintf(stderr, "Elapsed time: %lf s\n", elapsed);

    //--- Release resources

    dealloc_mapped_memory(threads, num_threads * sizeof(*threads));
    dealloc_mapped_memory(thread_data, num_threads * sizeof(*thread_data));
    dealloc_mapped_memory(res, n*sizeof(*res));
    dealloc_mapped_memory(vec, n*sizeof(*vec));
    dealloc_mapped_memory(mat, n*n*sizeof(*mat));

    return 0;
}

// --- <> ---

static void* matvec_worker(void* arg)
{
    const Thread_Data* data = (Thread_Data*)arg;
    const Chunk chunk = data->chunk;

    for (size_t i = chunk.start_index; i < chunk.end_index; i++) {
        // To reduce memory traffic to data[i]->res
        double accumulator = 0.0;

        for (size_t j = 0; j < data->n; j++) {
            accumulator += data->mat[i * data->n + j] * data->vec[j];
        }

        data->res[i] = accumulator;
    }

    return NULL;
}
