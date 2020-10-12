"""
Utility functions
"""

import numpy as np
from numpy.random import random

def sequence_1(N, dist):
    """sequence_1 returns a sequence of value distributed
    according to dist.

    :param N: int for the size of the sequence
    :param dist: array for the distribution
    """
    #dist must start at 0
    cum_dist = np.cumsum(dist)
    sequence = []
    for i in range(N):
        u = random()
        m = np.searchsorted(cum_dist, u)
        sequence.append(m)
    return sequence

def sequence_2(seq_1,dist):
    """sequence_2 returns a sequence of value distributed according
    to dist and is coherent with seq_1

    :param seq_1: array for sequence of int.
    :param dist: array for the distribution
    """
    seq_1_stub = np.sum(seq_1)
    #dist must start at 0
    cum_dist = np.cumsum(dist)
    seq_2 = []
    seq_2_stub = 0
    #generate new sequence number until the sum is coherent with seq_1
    while (seq_2_stub != seq_1_stub):
        if seq_2_stub < seq_1_stub:
            u = random()
            n = np.searchsorted(cum_dist, u)
            seq_2.append(n)
            seq_2_stub += n
        elif seq_2_stub > seq_1_stub:
            index = int(np.floor(random()*len(seq_2)))
            seq_2[index], seq_2[-1] = seq_2[-1], seq_2[index]
            seq_2_stub -= seq_2.pop()
    return seq_2

