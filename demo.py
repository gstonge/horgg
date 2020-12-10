import horgg
from scipy.special import loggamma
import numpy as np

#group
nmax = 4
pn = np.zeros(nmax+1)
pn[nmax] += 1

#membership
mmax = 3
gm = np.zeros(mmax+1)
gm[mmax] += 1

#generate sequences
np.random.seed(42) #optional, if nothing is given, it is seeded with time
N = 30 #number of groups
n_list = horgg.utility.sequence_1(N, pn)
m_list = horgg.utility.sequence_2(n_list, gm)

#define bipartie graph sampler
#if the sequences are not bigraphical, it (should) throws an error
graph_generator = horgg.BCMS(m_list,n_list)
horgg.BCMS.seed(42) #optional, if nothing is given, it is seeded with time

#mcmc steps are additional edge swaps to ensure uniformity, I recommend O(N)
edge_list = graph_generator.get_graph(mcmc_step=N)

print(edge_list)
