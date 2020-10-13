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

#include "GraphGenerator.hpp"
#include <algorithm>
#include <time.h>

using namespace std;

namespace horgg
{//start of namespace horgg

/* =================
 * BaseGenerator
 * ================= */
//initialize RNG

RNGType BaseGenerator::gen_ = RNGType(time(NULL));

//method to seed the RNG
void BaseGenerator::seed(unsigned int seed_value)
{
    BaseGenerator::gen_.seed(seed_value);
}

/* ========================================
 * BipartiteConfigurationModelSampler
 * ======================================== */

//Constructor of the bipartite configuration model generator
BipartiteConfigurationModelSampler::BipartiteConfigurationModelSampler(
        const MembershipSequence& membership_sequence,
        const GroupSizeSequence& group_size_sequence):
    group_stub_vector_(),node_stub_vector_(),
    largest_group_label_(group_size_sequence.size()-1),
    largest_node_label_(membership_sequence.size()-1)
{
    try
    {
        is_bigraphic(membership_sequence,group_size_sequence);
    }
    catch (invalid_argument& e)
    {
        throw e;
    }

    //initialize group and node stub vectors
    for (Node i = 0; i < membership_sequence.size(); i++)
    {
        for (int j = 0; j < membership_sequence[i]; j++)
        {
            node_stub_vector_.push_back(i);
        }
    }
    for (Group i = 0; i < group_size_sequence.size(); i++)
    {
        for (int j = 0; j < group_size_sequence[i]; j++)
        {
            group_stub_vector_.push_back(i);
        }
    }
}

//generate a random bipartite graph
Output BipartiteConfigurationModelSampler::get_graph(unsigned int mcmc_step,
        unsigned int max_attempts)
{
    //shuffle the stub vectors and get an edge list
    shuffle(group_stub_vector_.begin(),group_stub_vector_.end(),gen_);
    shuffle(node_stub_vector_.begin(),node_stub_vector_.end(),gen_);
    EdgeList edge_list;
    for (int i = 0; i < node_stub_vector_.size(); i++)
    {
        edge_list.push_back(make_pair(node_stub_vector_[i],
                    group_stub_vector_[i]));
    }

    // Check for repeated edges -- rewire them
    bool faulty_links = true;
    while (faulty_links)
    {
        faulty_links = false;
        sort(edge_list.begin(),edge_list.end());
        for (size_t edge1=0; edge1<edge_list.size(); edge1++)
        {
            // If the link is faulty, rewire the stubs
            if (edge_list[edge1] == edge_list[(edge1+1)% edge_list.size()])
            {
                faulty_links = true;
                Node node1 = edge_list[edge1].first;
                Group group1 = edge_list[edge1].second;
                unsigned int edge2 = random_int(edge_list.size(), gen_);
                Node node2 = edge_list[edge2].first;
                Group group2 = edge_list[edge2].second;
                // Switch stubs
                edge_list[edge1].second = group2;
                edge_list[edge2].second = group1;
            }
        }
    }

    //perform additional mcmc steps to ensure uniformity
    if (mcmc_step > 0)
    {
        EdgeSet edge_set(edge_list.begin(),edge_list.end());
        for(int i = 0; i < mcmc_step; i++)
        {
            bool new_link = false;
            unsigned int counter = 0;
            while (not new_link)
            {
                if (counter == max_attempts)
                {
                   throw runtime_error(
                           "Maximal number of edge swap attempts reached");
                }
                counter += 1;
                unsigned int edge1 = random_int(edge_list.size(), gen_);
                unsigned int edge2 = random_int(edge_list.size(), gen_);
                if (edge1 != edge2)
                {
                    Group group1 = edge_list[edge1].second;
                    Group group2 = edge_list[edge2].second;
                    // Switch stubs
                    edge_list[edge1].second = group2;
                    edge_list[edge2].second = group1;
                    // verify it has not created multi-edge
                    if (edge_set.count(edge_list[edge1]) > 0 or
                            edge_set.count(edge_list[edge2]) > 0)
                    {
                        //revert
                        edge_list[edge1].second = group1;
                        edge_list[edge2].second = group2;
                    }
                    else
                    {
                        new_link = true;
                    }
                }
            }
        }
    }

    //output the adjacency lists
    GroupAdjacency group_adjacency(largest_node_label_+1,vector<Group>());
    NodeAdjacency node_adjacency(largest_group_label_+1,vector<Node>());

    for (auto& edge : edge_list)
    {
        group_adjacency[edge.first].push_back(edge.second);
        node_adjacency[edge.second].push_back(edge.first);
    }
    return make_pair(group_adjacency,node_adjacency);
}

//verify if two sequences are bigraphic
bool BipartiteConfigurationModelSampler::is_bigraphic(
        const vector<unsigned int>& seq1,
        const vector<unsigned int>& seq2)
{
    bool bigraphic = true;
    if (accumulate(seq2.begin(),seq2.end(),0) !=
            accumulate(seq1.begin(),seq1.end(), 0))
    {
        bigraphic = false;
        throw invalid_argument("Sequences do not sum to the same value.");
    }
    //check the other condition for the Gale–Ryser theorem
    vector<unsigned int> seq1_copy(seq1);
    vector<unsigned int> seq2_copy(seq2);
    sort(seq1_copy.begin(),seq1_copy.end(),greater<unsigned int>()); //descend
    sort(seq2_copy.begin(),seq2_copy.end()); //ascend
    vector<unsigned int> cum_seq1;
    vector<unsigned int> cum_seq2;
    cum_seq1.push_back(seq1_copy[0]);
    for (int i = 1; i < seq1_copy.size(); i++)
    {
        cum_seq1.push_back(
                cum_seq1.back()+seq1_copy[i]);
    }
    cum_seq2.push_back(seq2_copy[0]);
    for (int i = 1; i < seq2_copy.size(); i++)
    {
        cum_seq2.push_back(
                cum_seq2.back()+seq2_copy[i]);
    }
    for (int k = 0; k < seq1_copy.size(); k++)
    {
        auto lower_it = lower_bound(seq2_copy.begin(),seq2_copy.end(),k+1);
        //cout << int(lower_it == seq2_copy.end()) << endl;
        unsigned int index = lower_it - seq2_copy.begin();
        //cout << "index " << index << endl;
        unsigned int sum = 0;
        if (index > 0)
        {
            sum += cum_seq2[index-1];
        }
        sum += (k+1)*(seq2_copy.size()-index);
        if (seq1_copy[k] > sum)
        {
            bigraphic = false;
            throw invalid_argument("Sequences are not bigraphic.");
        }
    }
    return bigraphic;
}


}//end of namespace horgg
