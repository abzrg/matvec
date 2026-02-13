#pragma once

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <sys/mman.h>


// @func now
// @return current time
// @example:
//
//     double t0 = now();
//     matvec_serial(mat, vec, res, n);
//     double t1 = now();
//
//     printf("Time: %f seconds\n", t1 - t0);
//
extern inline double now(void);

// @func print_vec
// @param vec the to be printed input vector
// @param n the size of the input vector
// @desc prints vectors (array of doubles)
extern inline void print_vec(const double* const vec, size_t n);

// @func alloc_private_memory
// @param size size in bytes of memory to be allocated
// @param prot the protection options (PROT_READ, PROT_WRITE)
// @desc Allocates private memory that can only be accessed in one process
extern void* alloc_private_memory(size_t size, int prot);

// @func alloc_shared_memory
// @param size size in bytes of memory to be allocated
// @param prot the protection options (PROT_READ, PROT_WRITE)
// @desc Allocates (maps) private memory that can be accessed in multiple
// process. This is useful for Inter-Process Communication purposes.
extern void* alloc_shared_memory(size_t size, int prot);

// @func dealloc_mapped_memory
// @param mem pointer to the mapped memory
// @param size size in bytes of the mapped memory (to be unmapped)
// @desc Deallocates mapped memory.
extern void dealloc_mapped_memory(void* mem, size_t size);

// @func ...
extern void print_mat(const double* mat, size_t m, size_t n);

// @func ...
extern void print_vec(const double* vec, size_t n);


#ifdef UTILS_IMPLEMENTATION

inline double now(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

void* alloc_private_memory(size_t size, int prot)
{
    void* mem = mmap(NULL, size, prot, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    assert(mem != MAP_FAILED);

    return mem;
}

extern void* alloc_shared_memory(size_t size, int prot)
{
    void* mem = mmap(NULL, size, prot, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    assert(mem != MAP_FAILED);

    return mem;
}

extern void dealloc_mapped_memory(void* mem, size_t size)
{
    int err = munmap(mem, size);
    assert(err >= 0);
}

extern void print_mat(const double* mat, size_t m, size_t n)
{
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            printf("%8.3lf", mat[i * n + j]);
        }
        putchar('\n');
    }
}

extern void print_vec(const double* vec, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        printf("%8.3lf", vec[i]);
    }
    putchar('\n');
}

#endif // UTILS_IMPLEMENTATION
