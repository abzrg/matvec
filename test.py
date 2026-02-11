import numpy as np


def reference(n):
    mat = np.fromfunction(lambda i, j: i + j, (n, n), dtype=float)
    vec = np.arange(n, dtype=float)
    reference_result = mat @ vec
    return reference_result


def main() -> int:
    with open("res.txt", "r") as f:
        result = [float(line.strip()) for line in f]

    reference_result = reference(len(result))
    print("Match:", np.allclose(reference_result, np.array(result),
                                rtol=1e-12, atol=1e-12))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
