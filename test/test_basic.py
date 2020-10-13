#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Unit tests

Author: Guillaume St-Onge <guillaume.st-onge.4@ulaval.ca>
"""

import pytest
import numpy as np
from horgg import BCMS

class TestOutput:
    """Tests for the properties of the generated bipartite graphs"""
    def test_sum_preserved_1(self):
        m_list = [2,2,2]
        n_list = [2,2,2]
        graph_generator = BCMS(m_list,n_list)
        group_adj,node_adj = graph_generator.get_graph()
        assert m_list == [len(group_adj[n]) for n in range(len(group_adj))]
        assert n_list == [len(node_adj[g]) for g in range(len(node_adj))]

    def test_sum_preserved_2(self):
        m_list = [1,2,3,4]
        n_list = [1,2,3,4]
        graph_generator = BCMS(m_list,n_list)
        group_adj,node_adj = graph_generator.get_graph()
        assert m_list == [len(group_adj[n]) for n in range(len(group_adj))]
        assert n_list == [len(node_adj[g]) for g in range(len(node_adj))]

    def test_different_graph_1(self):
        m_list = [2]*5
        n_list = [2]*5
        graph_generator = BCMS(m_list,n_list)
        BCMS.seed(42)
        group_adj_1,node_adj_1 = graph_generator.get_graph()
        graph_generator = BCMS(m_list,n_list)
        BCMS.seed(42)
        group_adj_2,node_adj_2 = graph_generator.get_graph(1)
        assert group_adj_1 != group_adj_2 and node_adj_1 != node_adj_2

    def test_different_graph_2(self):
        m_list = [2]*5
        n_list = [2]*5
        graph_generator = BCMS(m_list,n_list)
        BCMS.seed(42)
        group_adj_1,node_adj_1 = graph_generator.get_graph(1)
        graph_generator = BCMS(m_list,n_list)
        BCMS.seed(42)
        group_adj_2,node_adj_2 = graph_generator.get_graph(2)
        assert group_adj_1 != group_adj_2 and node_adj_1 != node_adj_2

    def test_throw_error_max_attempts(self):
        #there is one graph possible
        m_list = [1,2,3,4]
        n_list = [1,2,3,4]
        graph_generator = BCMS(m_list,n_list)
        with pytest.raises(RuntimeError):
            group_adj,node_adj = graph_generator.get_graph(
                mcmc_step=10,max_attempts=10)



class TestBigraphicSeq:
    """Tests to ensure that the Gale-Ryser theorem is respected"""
    def test_throw_error_1(self):
        #not the same sum
        m_list = [2,2,2]
        n_list = [2,3,2]
        with pytest.raises(ValueError):
            graph_generator = BCMS(m_list,n_list)

    def test_throw_error_2(self):
        #first m is too big
        m_list = [6,2,2]
        n_list = [4,4,2]
        with pytest.raises(ValueError):
            graph_generator = BCMS(m_list,n_list)

    def test_throw_error_3(self):
        #first n is too big
        m_list = [4,4,2]
        n_list = [6,2,2]
        with pytest.raises(ValueError):
            graph_generator = BCMS(m_list,n_list)

    def test_no_throw_1(self):
        m_list = [2,2,2]
        n_list = [2,2,2]
        graph_generator = BCMS(m_list,n_list)

    def test_no_throw_2(self):
        #limit case Gale–Ryser theorem
        m_list = [4,4]
        n_list = [2,2,2,2]
        graph_generator = BCMS(m_list,n_list)

    def test_no_throw_3(self):
        #limit case Gale–Ryser theorem
        m_list = [4,4,2]
        n_list = [2,2,2,2,2]
        graph_generator = BCMS(m_list,n_list)
