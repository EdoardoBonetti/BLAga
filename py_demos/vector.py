
import BLAga

v = BLAga.Vector(10)
v[:] = 5.0

view = v[2:8:2]
print(view)   # should print 5.0, 5.0, 5.0
view[0] = 42
print(v[2])   # should print 42, confirming shared memory



from BLAga.bla import Vector as blaVector
from ngsolve import Vector as ngVector


N = 10
ngx = ngVector(N)
ngy = ngVector(N)

blax = blaVector(N)
blay = blaVector(N)



for i in range(N):
    ngx[i] = i
    blax[i] = i


for i in range(N):
    ngy[i] = 2
    blay[i] = 2


print ("x =", blax)
print ("y =", blay)
print ("x+3*y =", blax+3*blay)

print("Check that they are the same:  .....")
# create a, b, c vectors of the same size n = 1001
# do it for ngsolve and bla to check that they are the same
n = 1001

nga = blaVector(n)
ngb = blaVector(n)
ngc = blaVector(n)

blaa = blaVector(n)
blab = blaVector(n)
blac = blaVector(n)

for i in range(n):
    nga[i] = i
    blaa[i] = i

for i in range(n):
    ngb[i] = 2
    blab[i] = 2

ngc = nga + 3*ngb
blac = blaa + 3*blab

#ngc[20:5:100] = 13.3
#blac[20:5:100] = 13.3

ngc[500:600] = 13.3
blac[500:600] = 13.3


ngc[10:20:100] = -100
blac[10:20:100] = -100

ngc[950:5:-1] = 13.3
blac[950:5:-1] = 13.3

corr = True
for i in range(20, 5, 100):
    if ngc[i] != blac[i]:
        corr = False

print("Are they the same: ", corr)


a = blaVector(10)
a[0:10:3] = 1
print(a)            # 1, 0, 1, 0, 1, 0, 1, 0, 1, 0
print(a[0:10:2])    # 1, 1, 1, 1, 1   ✅ correct placement
a[0:10:3][1] = 99
print(a[1])         # 99 ✅ same memory
print(a)
print(a[0:10:3])

a = blaVector(10)
for i in range(10):
    a[i]= i
print(a[0:20:3])
