import time
import matplotlib.pyplot as plt
from BLAga.bla import Matrix  # Your pybind11 wrapper
#from ngsolve import Matrix  # Using ngsolve's Matrix for demonstration
sizes = [255, 511, 1023, 2047]
data = []

for n in sizes:
    A = Matrix(n, n)
    B = Matrix(n, n)
    C = Matrix(n, n)


    # Initialize deterministically
    for i in range(n):
        for j in range(n):
            A[i, j] = 1.0 + i + j
            B[i, j] = 1.0 / (1.0 + i + j)

    iterations = 1
    start = time.time()
    for _ in range(iterations):
        C = A * B
    end = time.time()

    elapsed = (end - start) / iterations
    gflops = 2 * n**3 / (elapsed * 1e9)
    data.append((n, elapsed))

    print(f"Size: {n}x{n}, Time: {elapsed:.6f} s, GFLOPS: {gflops:.3f}")
    print("C[0,0] =", C[0,0])

# Plot execution times
sizes, times = zip(*data)
plt.figure(figsize=(8,6))
plt.plot(sizes, times, marker='o', color='b', label='Execution time')
plt.xscale('log')
plt.yscale('log')
plt.xlabel('Matrix size (N)')
plt.ylabel('Time (seconds)')
plt.title('Execution Time vs Matrix Size')
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.legend()
plt.tight_layout()
plt.show()
