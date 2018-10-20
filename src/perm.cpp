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

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "uasat/perm.hpp"

namespace uasat {

void Group::test_axioms() {
  if (contains(identity()).get_scalar() != Logic::TRUE) {
    std::cout << "does not contain the identity elem";
    std::cout << identity() << std::endl;
  }

  std::shared_ptr<Solver> solver = Solver::create();
  Tensor elem1 = Tensor::variable(solver, get_shape());
  solver->add_clause(contains(elem1)
                         .logic_leq(contains(inverse(elem1)))
                         .logic_not()
                         .get_scalar());
  if (solver->solve()) {
    std::cout << "not closed under taking the inverse" << std::endl;
    std::cout << elem1.get_solution(solver) << std::endl;
  }

  solver->clear();
  elem1 = Tensor::variable(solver, get_shape());
  Tensor elem2 = Tensor::variable(solver, get_shape());
  solver->add_clause(contains(elem1)
                         .logic_and(contains(elem2))
                         .logic_leq(contains(product(elem1, elem2)))
                         .logic_not()
                         .get_scalar());
  if (solver->solve()) {
    std::cout << "not closed under taking the product" << std::endl;
    std::cout << elem1.get_solution(solver) << std::endl;
    std::cout << elem2.get_solution(solver) << std::endl;
  }

  solver->clear();
  elem1 = Tensor::variable(solver, get_shape());
  solver->add_clause(
      contains(elem1)
          .logic_leq(product(identity(), elem1).logic_equ(elem1).fold_all())
          .logic_not()
          .get_scalar());
  if (solver->solve()) {
    std::cout << "left identity axiom is not satisfied" << std::endl;
    std::cout << elem1.get_solution(solver) << std::endl;
  }

  solver->clear();
  elem1 = Tensor::variable(solver, get_shape());
  solver->add_clause(
      contains(elem1)
          .logic_leq(
              product(inverse(elem1), elem1).logic_equ(identity()).fold_all())
          .logic_not()
          .get_scalar());
  if (solver->solve()) {
    std::cout << "left inverse axiom is not satisfied" << std::endl;
    std::cout << elem1.get_solution(solver) << std::endl;
  }

  solver->clear();
  elem1 = Tensor::variable(solver, get_shape());
  elem2 = Tensor::variable(solver, get_shape());
  Tensor elem3 = Tensor::variable(solver, get_shape());
  solver->add_clause(
      contains(elem1)
          .logic_and(contains(elem2))
          .logic_and(contains(elem3))
          .logic_leq(product(product(elem1, elem2), elem3)
                         .logic_equ(product(elem1, product(elem2, elem3)))
                         .fold_all())
          .logic_not()
          .get_scalar());
  if (solver->solve()) {
    std::cout << "associativity axiom is not satisfied" << std::endl;
    std::cout << elem1.get_solution(solver) << std::endl;
    std::cout << elem2.get_solution(solver) << std::endl;
    std::cout << elem3.get_solution(solver) << std::endl;
  }
}

SymmetricGroup::SymmetricGroup(int size) : size(size) {
  if (size <= 0)
    throw std::invalid_argument("size must be positive");
}

std::vector<int> SymmetricGroup::get_shape() const { return {size, size}; }

Tensor SymmetricGroup::contains(const Tensor &elem) {
  assert(elem.get_shape() == get_shape());
  return elem.fold_one({false, true})
      .fold_all()
      .logic_and(elem.fold_any({true, false}).fold_all());
}

Tensor SymmetricGroup::identity() { return Tensor::diagonal(size); }

Tensor SymmetricGroup::inverse(const Tensor &elem) {
  assert(elem.get_shape() == get_shape());
  return elem.polymer({size, size}, {1, 0});
}

Tensor SymmetricGroup::product(const Tensor &elem1, const Tensor &elem2) {
  assert(elem1.get_shape() == get_shape());
  assert(elem2.get_shape() == get_shape());
  return elem1.polymer({size, size, size}, {0, 1})
      .logic_and(elem2.polymer({size, size, size}, {1, 2}))
      .fold_any({false, true, false});
}

} // namespace uasat
