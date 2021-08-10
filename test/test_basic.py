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

    def test_different_graph_1(self):
        m_list = [2]*5
        n_list = [2]*5
        graph_generator = BCMS(m_list,n_list)
        BCMS.seed(42)
        edge_list1 = graph_generator.get_random_graph()
        graph_generator = BCMS(m_list,n_list)
        BCMS.seed(42)
        edge_list2 = graph_generator.get_random_graph(10)
        assert edge_list1 != edge_list2

    def test_different_graph_2(self):
        m_list = [2]*5
        n_list = [2]*5
        graph_generator = BCMS(m_list,n_list)
        BCMS.seed(42)
        edge_list1 = graph_generator.get_random_graph(10)
        graph_generator = BCMS(m_list,n_list)
        BCMS.seed(42)
        edge_list2 = graph_generator.get_random_graph(100)
        assert edge_list1 != edge_list2


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
