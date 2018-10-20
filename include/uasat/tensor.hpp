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

#ifndef UASAT_TENSOR_HPP
#define UASAT_TENSOR_HPP

#include <ostream>

#include "uasat/solver.hpp"

namespace uasat {

class Tensor {
protected:
  /**
   * The logic underlying this tensor.
   */
  std::shared_ptr<Logic> logic;

  /**
   * The shape of a tensor is a list of positive integers describing the
   * dimension of the tensor along the given axes. The rank of the tensor is the
   * number of axes.
   */
  std::vector<int> shape;

  /**
   * The elements of the tensor are stored in an array. Each element is
   * identified by an index within this array and also by a list of coordinates,
   * one for each axis.
   */
  std::vector<literal_t> storage;

  Tensor(const std::shared_ptr<Logic> &logic, const std::vector<int> &shape);

  /**
   * Performs the given generic binary logic operation on the given tensors.
   */
  Tensor logic_bin(literal_t (Logic::*op)(literal_t, literal_t),
                   const Tensor &tensor2) const;

  /**
   * Performs the given generic fold operation on the given tensor,
   * at the selected axes.
   */
  Tensor fold_bin(literal_t (Logic::*op)(const std::vector<literal_t> &),
                  const std::vector<bool> &selection) const;

  /**
   * Performs the given generic fold operation on all axes.
   */
  Tensor fold_bin(literal_t (Logic::*op)(const std::vector<literal_t> &)) const;

  /**
   * Returns the index of the element identified by the given coordinates.
   */
  size_t __very_slow_get_index(const std::vector<int> &coordinates) const;

public:
  /**
   * Returns the shape of this tensor.
   */
  std::vector<int> get_shape() const { return shape; }

  /**
   * Returns the literal in this tensor at the given coordinates.
   */
  literal_t __very_slow_get_value(const std::vector<int> &coords) const {
    return storage[__very_slow_get_index(coords)];
  }

  /**
   * Creates a new tensor with the given shape with fresh variables from
   * the selected solver.
   */
  static Tensor variable(const std::shared_ptr<Solver> &solver,
                         const std::vector<int> &shape, bool decision = true,
                         bool polarity = false);

  /**
   * Creates a new tensor with the given shape filled with the same literal.
   */
  static Tensor constant(const std::shared_ptr<Logic> &logic,
                         const std::vector<int> &shape, literal_t literal);

  /**
   * Creates the equality relation of shape (dimension, dimension).
   */
  static Tensor diagonal(int dimension);

  /**
   * Creates a new tensor of the given shape from the given old tensor with
   * permuted, identified or new coordinates. The mapping is a vector
   * of length of the old tensor shape with entries identifying the coordinate
   * in the new tensor.
   */
  Tensor polymer(const std::vector<int> &shape,
                 const std::vector<int> &mapping) const;

  /**
   * Creates a new tensor from the given tensor whose entries are negated.
   */
  Tensor logic_not() const;

  /**
   * Creates a new tensor from the given pair of tensors whose entries are
   * the logical and of the corresponding literals.
   */
  Tensor logic_and(const Tensor &tensor2) const {
    return logic_bin(&Logic::logic_and, tensor2);
  }

  /**
   * Creates a new tensor from the given pair of tensors whose entries are
   * the logical or of the corresponding literals.
   */
  Tensor logic_or(const Tensor &tensor2) const {
    return logic_bin(&Logic::logic_or, tensor2);
  }

  /**
   * Creates a new tensor from the given pair of tensors whose entries are
   * the implication of the corresponding literals.
   */
  Tensor logic_leq(const Tensor &tensor2) const {
    return logic_bin(&Logic::logic_leq, tensor2);
  }

  /**
   * Creates a new tensor from the given pair of tensors whose entries are
   * the logical sum of the corresponding literals.
   */
  Tensor logic_add(const Tensor &tensor2) const {
    return logic_bin(&Logic::logic_add, tensor2);
  }

  /**
   * Creates a new tensor from the given pair of tensors whose entries are
   * the logical xor of the corresponding literals.
   */
  Tensor logic_equ(const Tensor &tensor2) const {
    return logic_bin(&Logic::logic_equ, tensor2);
  }

  /**
   * Fold the given tensor along the selected axes using the logical and
   * operation.
   */
  Tensor fold_all(const std::vector<bool> &selection) const {
    return fold_bin(&Logic::fold_all, selection);
  }

  /**
   * Folds this tensor along all axes using the logical and operation and
   * returns a scalar tensor with the result.
   */
  Tensor fold_all() const { return fold_bin(&Logic::fold_all); }

  /**
   * Fold the given tensor along the selected axes using the logical or
   * operation.
   */
  Tensor fold_any(const std::vector<bool> &selection) const {
    return fold_bin(&Logic::fold_any, selection);
  }

  /**
   * Folds this tensor along all axes using the logical or operation and returns
   * a scalar tensor with the result.
   */
  Tensor fold_any() const { return fold_bin(&Logic::fold_any); }

  /**
   * Fold the given tensor along the selected axes using the exactly one is true
   * operation.
   */
  Tensor fold_one(const std::vector<bool> &selection) const {
    return fold_bin(&Logic::fold_one, selection);
  }

  /**
   * Folds this tensor along all axes using the exactly one is true operation
   * and returns a scalar tensor with the result.
   */
  Tensor fold_one() const { return fold_bin(&Logic::fold_one); }

  /**
   * Returns the scalar value of a zero rank tensor.
   */
  literal_t get_scalar() const;

  /**
   * Returns the boolean tensor that contains the values of this
   * tensor in the current solution.
   */
  Tensor get_solution(const std::shared_ptr<Solver> &solver) const;

  /**
   * Adds the literals of this tensor to the clause.
   */
  void get_clause(std::vector<literal_t> &clause) const;

  /**
   * Prints out the tensor content without shape information.
   */
  friend std::ostream &operator<<(std::ostream &out, const Tensor &tensor);
};

} // namespace uasat

#endif // UASAT_TENSOR_HPP
