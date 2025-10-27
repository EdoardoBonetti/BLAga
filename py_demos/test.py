from BLAga import Matrix as blaMatrix
import numpy as np

m= 10
p = 10
n = 10

npA = np.zeros((m, p))
npB = np.zeros((p, n))

blaA = blaMatrix(m,p)
blaB = blaMatrix(p,n)

diff = np.zeros((m,n))



for k in range(p):
    for i in range(m):
        val = np.random.rand()
        npA[i,k] = val
        blaA[i,k] = val
    for j in range(n):
        val = np.random.rand()
        npB[k,j] = val
        blaB[k,j] = val

npC = npA @ npB
blaC = blaA * blaB

print("Numpy C:")
print(npC)

print("BLAga C:")
print(blaC)


correct = True
for i in range(m):
    for j in range(n):
        if (_diff:= abs(npC[i, j] - blaC[i, j]) ) > 1e-8:
            diff[i,j] = _diff
            correct = False

            #print(f"Mismatch at C[{i},{j}]: numpy={npC[i,j]}, BLAga={blaC[i,j]}")


print("Difference matrix:")
print(diff)