from time import time
from BLAga.bla import Matrix as blaMatrix
from ngsolve import Matrix as ngsMatrix
import numpy as np

# M, P, N sizes random vectors of length 30 between 1 and 1000
M_sizes = np.random.randint(1, 10, size=3)
P_sizes = np.random.randint(1, 10, size=3)
N_sizes = np.random.randint(1, 10, size=3)

for M, P, N in zip(M_sizes, P_sizes, N_sizes):
    ngA = ngsMatrix(M, P)
    ngB = ngsMatrix(P, N)
    blaA = blaMatrix(M, P)
    blaB = blaMatrix(P, N)


    # Initialize matrices with random values
    for i in range(M):
        for j in range(P):
            val = np.random.rand()
            ngA[i, j] = val
            blaA[i, j] = val

    for i in range(P):
        for j in range(N):
            val = np.random.rand()
            ngB[i, j] = val
            blaB[i, j] = val

    # Perform multiplication using ngsolve
    ngC = ngA * ngB
    # Perform multiplication using BLAga
    blaC = blaA * blaB

    # Verify correctness
    correct = True
    for i in range(M):
        for j in range(N):
            if abs(ngC[i, j] - blaC[i, j]) > 1e-8:
                correct = False
                print(f"Mismatch at C[{i},{j}]: ngsolve={ngC[i,j]}, BLAga={blaC[i,j]}")
                #break
    if correct:
        print(f"Matrix multiplication correct for sizes M={M}, P={P}, N={N}")
    else:
        print(f"Matrix multiplication incorrect for sizes M={M}, P={P}, N={N}")


# create 2 random matrix 2048 x 2048 and multiply them

N = 2048
A = blaMatrix(N, N)
B = blaMatrix(N, N)
C = blaMatrix(N, N)

ngA = ngsMatrix(N, N)
ngB = ngsMatrix(N, N)
ngC = ngsMatrix(N, N)

for j in range(N):
    for i in range(N):
        val = np.random.rand()
        A[i, j] = val
        B[i, j] = val**2
        ngA[i, j] = val
        ngB[i, j] = val**2

start = time()
C = A * B
end = time()
print(f"Matrix multiplication took {end - start} seconds")

start = time()
ngC = ngA * ngB
end = time()
print(f"Matrix multiplication took {end - start} seconds")

correct = True
for j in range(N):
    for i in range(N):
        if abs(ngC[i, j] - C[i, j]) > 1e-8:
            correct = False
            print(f"Mismatch at C[{i},{j}]: ngsolve={ngC[i,j]}, BLAga={C[i,j]}")
            break

if correct:
    print("Matrix multiplication correct")
else:
    print("Matrix multiplication incorrect")