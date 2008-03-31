#!/usr/bin/env python
from numpy import append, array, dot, shape, transpose
from numpy.linalg import det, svd

def superpose(nodes1, nodes2, select1, select2):
    vecs1 = array(nodes1)[array(select1)]
    vecs2 = array(nodes2)[array(select2)]
    print vecs1, vecs2

    n_vec, vec_size = shape(vecs1)
    center1 = sum(vecs1, 0) / float(n_vec)
    center2 = sum(vecs2, 0) / float(n_vec)
    vecs1 -= center1
    vecs2 -= center2

    V, S, W_trans = svd(dot(transpose(vecs2), vecs1))

    is_reflection = (det(V) * det(W_trans)) < 0.0
    if is_reflection:
        V[-1,:] = V[-1,:] * (-1.0)

    optimal_rotation = dot(V, W_trans)
    return dot(array(nodes2) - center2, optimal_rotation) + center1

#a = [ (1,1), (4,4), (1,4) ]
#b = [ (0,3), (3,0), (3,3), (5,5) ]

#print superpose(a, b, (0,1,2), (0,1,2))
