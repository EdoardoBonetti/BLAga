# -*- coding: utf-8 -*-


from  BLAga.bla import Vector as blaVector
from ngsolve import Vector as ngVector


def test_vector():
    n = 101
    a = blaVector(n)
    b = ngVector(n)
    for i in range(n):
        a[i] = i
        b[i] = i

    for i in range(n):
        assert a[i] == b[i]


def test_vector_range():
    n = 101
    a = blaVector(n)
    b = ngVector(n)

    for begin in range(0,n):
        for end in range(begin, n+1):
            for step in range(begin+1, end ):
                c = a[begin:end:step]
                d = b[begin:end:step]

                for i in range(len(c)):
                    assert c[i] == d[i]
            
