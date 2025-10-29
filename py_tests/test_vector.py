# -*- coding: utf-8 -*-


from  BLAga.bla import Vector as blaVector
from ngsolve import Vector as ngVector


def test_vector():
    a = blaVector(3)
    b = ngVector(3)
    for i in range(3):
        a[i] = i
        b[i] = i

    for i in range(3):
        assert a[i] == b[i]

