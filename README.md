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

Matrix (square): $`n \times n`$
Vector: $`n \times 1`$

```math
TODO
```

## Algorithm analysis

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
- [ ] Pthread
- [ ] OpenMP
- [ ] MPI
- [ ] OpenCL
