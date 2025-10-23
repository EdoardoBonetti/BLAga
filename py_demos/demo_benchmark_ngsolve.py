from ngsolve import Matrix , TaskManager
import time

def benchmark_matrix_matrix_multiplication(iterations, m, k, n):
    vali = 2.0
    valj = 1.0

    # First matrix (m x k)
    mat1 = Matrix(m, k)
    for i in range(m):
        for j in range(k):
            mat1[i, j] = 1/(1+j)

    # Second matrix (k x n)
    mat2 = Matrix(k, n)
    for i in range(k):
        for j in range(n):
            mat2[i, j] = 1.0

    # Start timing
    start = time.perf_counter()

    for iter in range(iterations):
        # Force evaluation
        tmp = mat1 *mat2  # matrix multiplication

        # Assign to result
        #result = tmp.copy()

        if iter == iterations - 1:
            print(tmp[0, 0])

    end = time.perf_counter()
    elapsed = end - start

    print(f"Matrix multiplication , size: {m}x{k} * {k}x{n}, "
          f"iterations: {iterations}, elapsed time: {elapsed:.6f} s, "
          f"avg per multiplication: {elapsed / iterations:.6f} s\n")


if __name__ == "__main__":
    with TaskManager():
        # Small matrices
        benchmark_matrix_matrix_multiplication(1000, 10,10,10)

        # Medium matrices
        benchmark_matrix_matrix_multiplication(500, 50, 50, 50)

        # Large matrices
        benchmark_matrix_matrix_multiplication(100, 200, 200, 200)

        # Very large matrices
        benchmark_matrix_matrix_multiplication(1, 3000, 3000, 3000)


