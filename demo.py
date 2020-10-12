import horgg
from scipy.special import loggamma
import numpy as np

#group
nmax = 3
pn = np.zeros(nmax+1)
pn[nmax] += 1

#Poisson membership
mmin = 1
mmax = 100
m = np.arange(mmax+1)
param = 5
gm = np.exp(m*np.log(param) - loggamma(m+1))
gm[0:mmin] = 0.
gm /= np.sum(gm)

# #Power-law membership
# mmin = 4
# nmax = 3
# mmax = 100
# m = np.arange(mmax+1)
# a = -0.8
# b = 3.72
# c = 40
# gm = np.zeros(mmax+1)
# gm[mmin:] = np.exp(-m[mmin:]/c)/(a+m[mmin:])**b
# gm[0:mmin] = 0.
# gm /= np.sum(gm)

#generate sequences
np.random.seed(42) #optional, if nothing is given, it is seeded with time
N = 10 #number of facets
n_list = horgg.utility.sequence_1(N, pn)
m_list = horgg.utility.sequence_2(n_list, gm)

#define bipartie graph sampler
#if the sequences are not bigraphical, it (should) throws an error
graph_generator = horgg.BCMS(m_list,n_list)
horgg.BCMS.seed(42) #optional, if nothing is given, it is seeded with time

#mcmc steps are additional edge swaps to ensure uniformity, I recommend O(N)
output = graph_generator.get_graph(mcmc_step=N)
group_adjacency = output[0]
node_adjacency = output[1]
#group_adjaency[node] gives the label of the groups to which node belongs
#node_adjaency[group] gives the label of the nodes belonging to group

#you can recover a more detailed adjacency structure like this
node_to_node_adjacency = [[node_adjacency[g] for g in group_adjacency[n]]
                          for n in range(len(m_list))]
print(node_to_node_adjacency)
#note that the concerned node is also included in the groups
