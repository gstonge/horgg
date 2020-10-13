/*
 * MIT License
 *
 * Copyright (c) 2020 Guillaume St-Onge
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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "GraphGenerator.hpp"

using namespace std;
using namespace horgg;

namespace py = pybind11;


PYBIND11_MODULE(_horgg, m)
{
    py::class_<BipartiteConfigurationModelSampler>(m,
            "BCMS")

        .def(py::init<vector<unsigned int>, vector<unsigned int> >(), R"pbdoc(
            Default constructor of the class BCMS

            Args:
               membership_sequence: Sequence of membership
               group_size_sequence: Sequence of group size
            )pbdoc", py::arg("membership_sequence"),
                py::arg("group_size_sequence"))

        .def_static("seed", &BaseGenerator::seed, R"pbdoc(
            Seed the RNG with a new value.

            Args:
               seed_value: New value for the seed of the RNG.
            )pbdoc", py::arg("seed_value"))

        .def("get_graph", &BipartiteConfigurationModelSampler::get_graph,
                R"pbdoc(
            Create a random edge list from the configuration model.

            Args:
               mcmc_step: unsigned int for the number of edge swaps to perform
               max_attempts: unsigned int for the max number of edge swaps
                             attempts for a single mcmc step.
            )pbdoc", py::arg("mcmc_step") = 0, py::arg("max_attempts") = 1000);
}
