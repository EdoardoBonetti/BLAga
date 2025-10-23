#from ngsolve import *
from time import time
from BLAga.bla import Matrix

n = 1

data = []
while n <= 1024*2:
    n = 2*n

    A = Matrix(n,n)
    B = Matrix(n,n)
    runs =  1+int(min( 1e8 / n**3, 1000))

    ts = time()
    for i in range(runs):
        C = A*B
    te = time()
    print ('n = ', n, ' time = ', (te-ts)/runs , ' GFLOPS = ', 2*n**3/runs/1e9)
    data.append( (n, (te-ts)/runs) )

print (data)


import matplotlib.pyplot as plt

sizes, times = zip(*data)

plt.figure(figsize=(8,6))
plt.plot(sizes, times, marker='o', linestyle='-', color='b', label='Execution time')

plt.xscale('log') # , base=2)
plt.yscale('log')

plt.xlabel('Matrix size (N)')
plt.ylabel('Time (seconds)')
plt.title('Execution Time vs. Matrix Size')
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.legend()
plt.tight_layout()
plt.show()



    