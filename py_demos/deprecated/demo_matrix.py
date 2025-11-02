# search for libraray like bla.cpython-312-darwin.so in the build directory:
# import sys
# sys.path.append('/Users/joachim/texjs/lva/ws2324/ScientificComputing/ASC-bla/build')
# from bla import Vector

# import from the installed package:
#from BLAga.bla import Vector
from BLAga.bla import Matrix
import numpy as np

n = 103

x = Matrix(n, n)
y = Matrix(n, n)
xplusy = Matrix(n, n)
npx = np.zeros((n, n))
npy = np.zeros((n, n))
npxplusnpy = np.zeros((n, n))

for i in range(n):
    for j in range(n):
        x[i, j] = max(i, j)
        y[i, j] = 10 *i + j
        npx[i, j] = max(i, j)
        npy[i, j] = 10 *i + j


xplusy = x + 3*y
npxplusnpy = npx + 3*npy

# assert that x equals npx
for i in range(n):
    for j in range(n):
        assert x[i, j] == npx[i, j]
        assert y[i, j] == npy[i, j]
        assert xplusy[i, j] == npxplusnpy[i, j]


p = 8
q = 3

mat_1 = Matrix(p, q)
np_1 = np.zeros((p, q))
mat_2 = Matrix(q, p)
np_2 = np.zeros((q, p))


for i in range(p):
    for j in range(q):
        mat_1[i, j] = i
        mat_2[j, i] = j
        np_1[i, j] = i
        np_2[j, i] = j


mat_3 = mat_1 * mat_2
np_3 = np_1.dot(np_2)

for i in range(p):
    for j in range(q):
        assert mat_3[i, j] == np_3[i, j]


M , N = 4, 4
a = Matrix( M, N)
for i in range(M):
    for j in range(N):
        a[i, j] = i*10 + j

print(a)
#print(a.Flatten())
for i in range(a.nRows()):
    print(a.Row(i))

for j in range(a.nCols()):
    print(a.Col(j))