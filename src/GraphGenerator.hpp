/*
 * MIT License
 *
 * Copyright (c) 2018 Guillaume St-Onge
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef GRAPH_GENERATOR_HPP_
#define GRAPH_GENERATOR_HPP_

#include "pcg-cpp/include/pcg_random.hpp"
#include <utility>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <random>
#include <iostream>
#include <string>
#include <stdexcept>
#include <numeric>
#include <limits>
#include <cmath>
#include "hash_specialization.hpp"

namespace horgg
{//start of namespace horgg

typedef unsigned int Node, Group;
typedef pcg32 RNGType;
typedef std::vector<std::pair<Node,Group> > EdgeList;
typedef std::unordered_set<std::pair<Node,Group> > EdgeSet;
typedef std::vector<unsigned int> MembershipSequence;
typedef std::vector<unsigned int> GroupSizeSequence;

//Base class to contain the shared RNG for derived template classes
class BaseGenerator
{
    public:
        static void seed(unsigned int seed_value);
    protected:
        static RNGType gen_;
};

/*
 * Class to sample bipartite configuration model graphs using MCMC
 */
class BipartiteConfigurationModelSampler : public BaseGenerator
{
public:
    BipartiteConfigurationModelSampler(
            const MembershipSequence& membership_sequence,
            const GroupSizeSequence& group_size_sequence);

    //graph generation methods
    EdgeList get_graph(unsigned int mcmc_step=0, unsigned int max_attempts=1000);

private:
    bool is_bigraphic(const std::vector<unsigned int>& seq1,
        const std::vector<unsigned int>& seq2);
    //members
    unsigned int largest_group_label_;
    unsigned int largest_node_label_;
    std::vector<Group> group_stub_vector_;
    std::vector<Node> node_stub_vector_;
};

inline unsigned int random_int(std::size_t size, RNGType& gen)
{
    return floor(std::generate_canonical<double,
        std::numeric_limits<double>::digits>(gen)*size);
}


}//end of namespace horgg

#endif /* GRAPH_GENERATOR_HPP_ */
