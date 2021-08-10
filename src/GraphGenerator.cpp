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

//Constructor using membership and group size sequences
BipartiteConfigurationModelSampler::BipartiteConfigurationModelSampler(
        const MembershipSequence& membership_sequence,
        const GroupSizeSequence& group_size_sequence):
    group_stub_vector_(),node_stub_vector_(),
    largest_group_label_(group_size_sequence.size()-1),
    largest_node_label_(membership_sequence.size()-1),
    edge_list_(),
    edge_set_()
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

    //initialize with stub matching
    stub_matching();
}


//Constructor using membership and group size sequences
BipartiteConfigurationModelSampler::BipartiteConfigurationModelSampler(
        const EdgeList& edge_list):
    group_stub_vector_(),node_stub_vector_(),
    largest_group_label_(0),
    largest_node_label_(0),
    edge_list_(edge_list),
    edge_set_(edge_list_.begin(),edge_list_.end())
{
    //determine largest labels and initialize node and group stub vector
    for (auto& edge : edge_list_)
    {
        Node node = edge.first;
        Group group = edge.second;
        if (node > largest_node_label_)
        {
            largest_node_label_ = node;
        }
        if (group > largest_group_label_)
        {
            largest_group_label_ = group;
        }
        node_stub_vector_.push_back(node);
        group_stub_vector_.push_back(group);
    }
}


//make the edge list the result of a stub matching
void BipartiteConfigurationModelSampler::stub_matching()
{
    //shuffle the stub vectors and get a new edge list
    shuffle(group_stub_vector_.begin(),group_stub_vector_.end(),gen_);
    shuffle(node_stub_vector_.begin(),node_stub_vector_.end(),gen_);
    edge_list_ = EdgeList();
    for (int i = 0; i < node_stub_vector_.size(); i++)
    {
        edge_list_.push_back(make_pair(node_stub_vector_[i],
                    group_stub_vector_[i]));
    }

    // Check for repeated edges -- rewire them
    bool faulty_links = true;
    while (faulty_links)
    {
        faulty_links = false;
        sort(edge_list_.begin(),edge_list_.end());
        for (size_t edge1=0; edge1<edge_list_.size(); edge1++)
        {
            // If the link is faulty, rewire the stubs
            if (edge_list_[edge1] == edge_list_[(edge1+1)% edge_list_.size()])
            {
                faulty_links = true;
                Node node1 = edge_list_[edge1].first;
                Group group1 = edge_list_[edge1].second;
                unsigned int edge2 = random_int(edge_list_.size(), gen_);
                Node node2 = edge_list_[edge2].first;
                Group group2 = edge_list_[edge2].second;
                // Switch stubs
                edge_list_[edge1].second = group2;
                edge_list_[edge2].second = group1;
            }
        }
    }

    edge_set_ = EdgeSet(edge_list_.begin(),edge_list_.end());
}

void BipartiteConfigurationModelSampler::mcmc_step()
{
    bool same_edge = true;
    while (same_edge)
    {
        unsigned int edge1 = random_int(edge_list_.size(), gen_);
        unsigned int edge2 = random_int(edge_list_.size(), gen_);
        if (edge1 != edge2)
        {
            same_edge = false;
            Node node1 = edge_list_[edge1].first;
            Node node2 = edge_list_[edge2].first;
            Group group1 = edge_list_[edge1].second;
            Group group2 = edge_list_[edge2].second;
            if (edge_set_.count(make_pair(node1,group2)) == 0
                    and edge_set_.count(make_pair(node2,group1)) == 0)
            {
                // Switch stubs
                edge_list_[edge1].second = group2;
                edge_list_[edge2].second = group1;
                //remove old edges and add new ones
                edge_set_.erase(make_pair(node1,group1));
                edge_set_.erase(make_pair(node2,group2));
                edge_set_.insert(make_pair(node1,group2));
                edge_set_.insert(make_pair(node2,group1));
            }
            //otherwise do nothing (the move is rejected)
        }
    }
}

//generate a random bipartite graph -- uses stub matching + mcmc
const EdgeList& BipartiteConfigurationModelSampler::get_random_graph(
        unsigned int nb_steps)
{
    //start from a new stub matching process
    stub_matching();

    //perform additional mcmc steps to ensure uniformity
    for(int i = 0; i < nb_steps; i++)
    {
        mcmc_step();
    }

    return edge_list_;
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
    //patch shortest sequence with 0s
    if (seq1_copy.size() < seq2_copy.size())
    {
        for (int i = 0; i < (seq2_copy.size() - seq1_copy.size()); i++)
        {
            seq1_copy.push_back(0);
        }
    }
    else
    {
        for (int i = 0; i < (seq1_copy.size() - seq2_copy.size()); i++)
        {
            seq2_copy.push_back(0);
        }
    }
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
