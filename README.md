# Matrix-Vector Multiplication

## Build

```console
$ make
```

Assuming the `bear` program is installed, to generate compilation database,
`compile_command.json`:

```console
$ make bear
```

## Math

Matrix (square): $`n \times n`$ (n: `double`)
Vector: $`n \times 1`$

```math
\begin{align}
y                = A x \\
\rightarrow y_i &= \sum_j^n A_{ij} x_j
\end{align}
```

## Algorithm analysis

```c
for (i = 0; i < 4; i++) {
    y[i] = 0;
    for (j = 0; j < 4; j++) {
        y[i] += A[i][j] * x[j];
    }
}
```

- Complexity: O(n²)
- Memory usage (assuming double precision): n² * 8 bytes

    | N      | Memory (matrix only) |
    | ------ | -------------------- |
    | 1'000  | 8 MB                 |
    | 5'000  | 200 MB               |
    | 10'000 | 800 MB               |
    | 20'000 | 3.2 GB               |

## TODO

- [X] serial (C)
- [ ] serial (assembly)
- [X] IPC via mapped buffers (Unix)
- [X] Pthread
- [ ] OpenMP
- [ ] MPI
- [ ] OpenCL
