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

#include "uasat/group.hpp"
#include "uasat/tensor.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace uasat {

void AbstractGroup::test_axioms() {
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
  solver->add_clause(contains(elem1)
                         .logic_leq(equals(product(identity(), elem1), elem1))
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
          .logic_leq(equals(product(inverse(elem1), elem1), identity()))
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
          .logic_leq(equals(product(product(elem1, elem2), elem3),
                            product(elem1, product(elem2, elem3))))
          .logic_not()
          .get_scalar());
  if (solver->solve()) {
    std::cout << "associativity axiom is not satisfied" << std::endl;
    std::cout << elem1.get_solution(solver) << std::endl;
    std::cout << elem2.get_solution(solver) << std::endl;
    std::cout << elem3.get_solution(solver) << std::endl;
  }
}

SymmetricGroup::SymmetricGroup(int size)
    : AbstractGroup({size, size}), size(size) {
  if (size < 0)
    throw std::invalid_argument("size must be non-negative");
}

Tensor SymmetricGroup::contains(const Tensor &elem) {
  assert(check_shape(elem.get_shape()));
  return elem.fold_one().fold_all().logic_and(
      inverse(elem).fold_any().fold_all());
}

Tensor SymmetricGroup::identity() { return Tensor::diagonal(size); }

Tensor SymmetricGroup::inverse(const Tensor &perm) {
  assert(check_shape(perm.get_shape()));
  return perm.polymer(shape, {1, 0});
}

Tensor SymmetricGroup::product(const Tensor &perm1, const Tensor &perm2) {
  assert(check_shape(perm1.get_shape()) && check_shape(perm2.get_shape()));
  return perm1.polymer({size, size, size}, {1, 0})
      .logic_and(perm2.polymer({size, size, size}, {0, 2}))
      .fold_any();
}

Tensor SymmetricGroup::even(const Tensor &perm) {
  assert(check_shape(perm.get_shape()));
  Tensor less = Tensor::lessthan(size);
  Tensor rel1 = less.polymer({size, size, size}, {1, 0})
                    .logic_and(perm.polymer({size, size, size}, {0, 2}))
                    .fold_any();
  Tensor rel2 = less.polymer({size, size, size}, {2, 0})
                    .logic_and(perm.polymer({size, size, size}, {1, 0}))
                    .fold_any();
  return rel1.logic_and(rel2).reshape(2, {size * size}).fold_sum();
}

BinaryNumAddition::BinaryNumAddition(int length)
    : AbstractGroup({length}), length(length) {
  if (length < 0)
    throw std::invalid_argument("length must be non-negative");
}

Tensor BinaryNumAddition::contains(const Tensor &elem) {
  assert(check_shape(elem.get_shape()));
  (void)elem;
  return Tensor::constant({}, true);
}

Tensor BinaryNumAddition::identity() {
  return Tensor::constant(get_shape(), false);
}

Tensor BinaryNumAddition::inverse(const Tensor &elem) {
  assert(check_shape(elem.get_shape()));
  return increment(elem.logic_not(), Tensor::constant({}, true));
}

Tensor BinaryNumAddition::product(const Tensor &elem1, const Tensor &elem2) {
  assert(check_shape(elem1.get_shape()) && check_shape(elem2.get_shape()));

  std::vector<Tensor> bits1 = elem1.slices();
  std::vector<Tensor> bits2 = elem2.slices();
  std::vector<Tensor> result;

  Tensor carry = Tensor::constant({}, false);
  for (size_t i = 0; i < bits1.size(); i++) {
    result.push_back(bits1[i].logic_add(bits2[i]).logic_add(carry));
    if (i + 1 < bits1.size())
      carry = bits1[i].logic_maj(bits2[i], carry);
  }

  return Tensor::stack(result);
}

Tensor BinaryNumAddition::constant(unsigned long value) {
  assert(length <= 64);

  std::vector<Tensor> bits;
  for (int i = 0; i < length; i++) {
    bool b = ((value >> i) & 1) != 0;
    bits.push_back(Tensor::constant({}, b));
  }

  return Tensor::stack(bits);
}

Tensor BinaryNumAddition::increment(const Tensor &elem, const Tensor &flag) {
  assert(check_shape(elem.get_shape()));

  std::vector<Tensor> bits = elem.slices();
  std::vector<Tensor> result;

  Tensor carry = flag;
  for (size_t i = 0; i < bits.size(); i++) {
    result.push_back(bits[i].logic_add(carry));
    if (i + 1 < bits.size())
      carry = bits[i].logic_and(carry);
  }

  return Tensor::stack(result);
}

Tensor BinaryNumAddition::weight(const Tensor &elem) {
  assert(check_shape(elem.get_shape()));

  Tensor result = identity();
  std::vector<Tensor> bits = elem.slices();
  for (size_t i = 0; i < bits.size(); i++)
    result = increment(result, bits[i]);

  return result;
}

} // namespace uasat
