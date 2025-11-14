# -*- coding: utf-8 -*-



#import pytest

from  BLAga import Vector as blaVector
from ngsolve import Vector as ngVector

from BLAga import Matrix as blaMatrix
from ngsolve import Matrix as ngMatrix
import numpy as np

tol = 1e-12

def test_matrix_zero_initializer():
    M, N = 101, 103
    blaA = blaMatrix(N, N)

    for i in range(N):
        for j in range(N):
            assert blaA[i, j] == 0 


def test_row():
    M, N = 3, 5
    a = blaMatrix(M, N)
    b = ngMatrix(M, N)  

    for i in range(M):
        for j in range(N):
            val = np.random.rand()
            a[i, j] = i*10 + j
            b[i, j] = i*10 + j

    print("my mat")
    print(a)
    print("ng mat")
    print(b)

    for i in range(M):
        print(a.Row(i))
        for j in range(N):
            
            assert a.Row(i)[j] == b[i,j]
    
    
def test_col():
    M, N = 3, 5
    a = blaMatrix(M, N)
    b = ngMatrix(M, N)  

    for i in range(M):
        for j in range(N):
            val = np.random.rand()
            a[i, j] = i*10 + j
            b[i, j] = i*10 + j

    for col in range(N):
        for row in range(M):
            assert a.Col(col)[row] == b[row,col]

def CompareMatrix(bla_a, ngs_b):
    a = bla_a
    b = ngs_b

    assert a.nRows() == len(b[:,0])
    assert a.nCols() == len(b[0,:])

    for i in range(a.nRows()):
        for j in range(a.nCols()):
            assert (a[i, j] -b[i, j])**2 < tol**2


def test_rows():
    M, N = 13, 7
    for M, N in [(M, N) for M in range(1, 17) for N in range(1, 17)]:
        a = blaMatrix(M, N)
        b = ngMatrix(M, N)  

        for i in range(M):
            for j in range(N):
                val = np.random.rand()
                a[i, j] = i*10 + j
                b[i, j] = i*10 + j


        for start in range(M-1):
            for end in range(start, M-1):
                print(start, end)
                ar = a.Rows(start, end)
                br = b[start:end, :]
                print(ar)
                print(br)

                CompareMatrix(ar, br)
        
def test_cols():
    M, N = 13, 7
    for M, N in [(M, N) for M in range(1, 17) for N in range(1, 17)]:
        a = blaMatrix(M, N)
        b = ngMatrix(M, N)

        # Fill both matrices with the same pattern
        for i in range(M):
            for j in range(N):
                val = np.random.rand()
                a[i, j] = i * 10 + j
                b[i, j] = i * 10 + j

        # Test all column ranges
        for start in range(N - 1):
            for end in range(start, N - 1):
                print(start, end)
                ac = a.Cols(start, end)
                bc = b[:, start:end]
                print(ac)
                print(bc)

                CompareMatrix(ac, bc)


#def test_matrix_transpose():
#    M, N = 101, 103
#    blaA = blaMatrix(M, N)
#    ngA = blaMatrix(M, N)
#
#    
#
#    for i in range(M):
#        for j in range(N):
#            val = np.random.rand()
#            blaA[i, j] = j + i*N
#            ngA[i, j] = j + i*N
#    print(blaA.nCols())
#    blaB = blaA.Transpose()
#    ngB = ngA.Transpose()
#
#    for i in range(N):
#        for j in range(M):
#            assert blaB[i, j] == ngB[i, j]
#

#def test_matrix():
#
#    # create a random vectro M of length 10 of random values between 1 and 600
#    M = np.random.randint(1, 600, size=10)
#    N = np.random.randint(1, 600, size=10)
#    P = np.random.randint(1, 600, size=10)
#
#    for (m,p,n) in zip(M, P, N):
#        ngA = ngMatrix(m, p)
#        ngB = ngMatrix(p, n)
#        blaA = blaMatrix(m, p)
#        blaB = blaMatrix(p, n)
#
#        # Initialize matrices with random values
#        for i in range(m):
#            for j in range(p):
#                val = np.random.rand()
#                ngA[i, j] = val
#                blaA[i, j] = val
#
#        for i in range(p):
#            for j in range(n):
#                val = np.random.rand()
#                ngB[i, j] = val
#                blaB[i, j] = val
#
#        ngC = ngA * ngB
#        blaC = blaA * blaB
#
#        for i in range(m):
#            for j in range(n):
#                assert ngC[i, j] == blaC[i, j]
#    

if __name__ == "__main__":
    test_matrix_zero_initializer()
    #test_matrix_transpose()
    test_col()
    #test_matrix()