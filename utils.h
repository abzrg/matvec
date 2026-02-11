#pragma once

#include <assert.h>
#include <stdio.h>
#include <time.h>


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

#ifdef UTILS_IMPLEMENTATION

inline double now(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

inline void print_vec(const double* const vec, size_t n)
{
    assert(vec);

    for (size_t i = 0; i < n; i++) {
        printf("%lf\n", vec[i]);
    }
}

#endif // UTILS_IMPLEMENTATION
