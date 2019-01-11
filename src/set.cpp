/*
 * Copyright (c) 2016-2018, Miklos Maroti, University of Szeged
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "uasat/set.hpp"
#include <cassert>
#include <stdexcept>

namespace uasat {

Set::Set(const std::vector<int> &shape) : shape(shape) {
  tensor_size = 1;

  for (int dimension : shape)
    tensor_size *= dimension;
}

Tensor Set::equals(const Tensor &elem1, const Tensor &elem2) {
  return elem1.logic_equ(elem2).reshape(shape.size(), {tensor_size}).fold_all();
}

Tensor Set::find_elements() {
  std::shared_ptr<Solver> solver = Solver::create();
  Tensor elem = Tensor::variable(solver, get_shape());
  solver->add_clause(contains(elem).get_scalar());

  std::vector<Tensor> elems;
  while (solver->solve()) {
    Tensor t = elem.get_solution(solver);
    elems.push_back(t);

    std::vector<uasat::literal_t> clause;
    elem.logic_add(t).extend_clause(clause);
    solver->add_clause(clause);
  }

  return Tensor::stack(elems);
}

int Set::find_cardinality() {
  std::shared_ptr<Solver> solver = Solver::create();
  Tensor elem = Tensor::variable(solver, get_shape());
  solver->add_clause(contains(elem).get_scalar());

  int card = 0;
  while (solver->solve()) {
    card += 1;

    std::vector<uasat::literal_t> clause;
    elem.logic_add(elem.get_solution(solver)).extend_clause(clause);
    solver->add_clause(clause);
  }

  return card;
}

} // namespace uasat
