import numpy as np
import time

def benchmark_matrix_matrix_multiplication(iterations, m, k, n, order_a='C', order_b='C'):
    vali = 2.0
    valj = 1.0

    # First matrix (m x k)
    mat1 = np.empty((m, k), order=order_a)
    for i in range(m):
        for j in range(k):
            mat1[i, j] = vali * i + valj * j

    # Second matrix (k x n)
    mat2 = np.empty((k, n), order=order_b)
    for i in range(k):
        for j in range(n):
            mat2[i, j] = 1.0 + 1.0 / (vali * i + valj * j + 3.2)

    # Start timing
    start = time.perf_counter()

    for iter in range(iterations):
        # Force evaluation
        tmp = mat1 @ mat2  # matrix multiplication

        # Assign to result
        result = tmp.copy()

        if iter == iterations - 1:
            print(result[0, 0])

    end = time.perf_counter()
    elapsed = end - start

    print(f"Matrix multiplication ({'ColMajor' if order_a=='F' else 'RowMajor'} x "
          f"{'ColMajor' if order_b=='F' else 'RowMajor'}), size: {m}x{k} * {k}x{n}, "
          f"iterations: {iterations}, elapsed time: {elapsed:.6f} s, "
          f"avg per multiplication: {elapsed / iterations:.6f} s\n")



if __name__ == "__main__":
    # Small matrices
    benchmark_matrix_matrix_multiplication(1000, 10,10,10)

    # Medium matrices
    benchmark_matrix_matrix_multiplication(500, 50, 50, 50)

    # Large matrices
    benchmark_matrix_matrix_multiplication(100, 200, 200, 200)

    # Very large matrices
    benchmark_matrix_matrix_multiplication(5, 2000, 2000, 2000)


