import horgg
from scipy.special import loggamma
import numpy as np

def get_node_to_node_adjacency(edge_list):
    nb_nodes = 0
    nb_groups = 0
    for node,group in edge_list:
        if node > nb_nodes:
            nb_nodes = node
        if group > nb_groups:
            nb_groups = group
    nb_nodes += 1
    nb_groups += 1
    group_list = [[] for i in range(nb_groups)]
    for node,group in edge_list:
        group_list[group].append(node)
    node_to_node_adjacency_list = [[] for i in range(nb_nodes)]
    for node,group in edge_list:
        node_to_node_adjacency_list[node].append(group_list[group])

    return node_to_node_adjacency_list

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

#get node adjacency
node_to_node_adjacency = get_node_to_node_adjacency(edge_list)
print(node_to_node_adjacency)
