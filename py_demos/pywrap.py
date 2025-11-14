import time
from BLAga import GEMM, Matrix

A = Matrix(512, 512)
B = Matrix(512, 512)
C = Matrix(512, 512)

start = time.perf_counter()
GEMM(A, B, C)
end = time.perf_counter()

print("Python call -> C++ GEMM:", end - start)

start = time.perf_counter()
A = Matrix(512, 512)
end = time.perf_counter()
print("Matrix creation:", end - start)

import cProfile
cProfile.run("GEMM(A, B, C)")