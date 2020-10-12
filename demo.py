import horgg
from scipy.special import loggamma
import numpy as np

#group
nmax = 3
pn = np.zeros(nmax+1)
pn[nmax] += 1

# #Poisson membership
# mmin = 1
# mmax = 100
# m = np.arange(mmax+1)
# param = 5
# gm = np.exp(m*np.log(param) - loggamma(m+1))
# gm[0:mmin] = 0.
# gm /= np.sum(gm)

#Power-law membership
mmin = 4
nmax = 3
mmax = 100
m = np.arange(mmax+1)
a = -0.8
b = 3.72
c = 40
gm = np.zeros(mmax+1)
gm[mmin:] = np.exp(-m[mmin:]/c)/(a+m[mmin:])**b
gm[0:mmin] = 0.
gm /= np.sum(gm)

#generate sequences
np.random.seed(42)
N = 20 #number of facets
n_list = horgg.utility.sequence_1(N, pn)
m_list = horgg.utility.sequence_2(n_list, gm)

#get graph
graph_generator = horgg.BCMS(m_list,n_list)
horgg.BCMS.seed(42) #optional, if nothing is given, it is seeded with time

output = graph_generator.get_graph(mcmc_step=10**3) #additional edge_swap to
                                                    #ensure uniformity
print('Group adjacency',output[0])
print('Node adjacency',output[1])
