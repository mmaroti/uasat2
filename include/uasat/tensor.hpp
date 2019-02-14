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

#include "solver.hpp"
#include <ostream>

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
   * Performs the given generic ternary logic operation on the given tensors.
   */
  Tensor logic_ter(literal_t (Logic::*op)(literal_t, literal_t, literal_t),
                   const Tensor &tensor2, const Tensor &tensor3) const;

  /**
   * Returns the index of the element identified by the given coordinates.
   */
  size_t __very_slow_get_index(const std::vector<int> &coordinates) const;

public:
  /**
   * Returns the shape of this tensor.
   */
  const std::vector<int> &get_shape() const { return shape; }

  /**
   * Returns the underlying logic object.
   */
  std::shared_ptr<Logic> get_logic() const { return logic; }

  /**
   * Returns the literal in this tensor at the given coordinates.
   */
  literal_t __very_slow_get_value(const std::vector<int> &coords) const {
    return storage[__very_slow_get_index(coords)];
  }

  /**
   * Returns the literal in this tensor at the given coordinates.
   */
  void __very_slow_set_value(const std::vector<int> &coords,
                             literal_t literal) {
    storage[__very_slow_get_index(coords)] = literal;
  }

  /**
   * Creates a new tensor with the given shape with fresh variables from the
   * selected solver.
   */
  static Tensor variable(const std::shared_ptr<Solver> &solver,
                         const std::vector<int> &shape, bool decision = true,
                         bool polarity = false);

  /**
   * Creates a new tensor with the given shape filled with the same value.
   */
  static Tensor constant(const std::vector<int> &shape, bool value);

  /**
   * Creates the equality relation of shape (dimension, dimension).
   */
  static Tensor diagonal(int dimension);

  /**
   * Creates the less than relation of shape (dimension, dimension).
   */
  static Tensor lessthan(int dimension);

  /**
   * Creates a new tensor of the given shape from the given old tensor with
   * permuted, identified or new dummy coordinates. The mapping is a vector of
   * length of the old tensor shape with entries identifying the coordinate in
   * the new tensor.
   */
  Tensor polymer(const std::vector<int> &shape,
                 const std::vector<int> &mapping) const;

  /**
   * Reshapes the first rank many axes of the tensor to dims so that the number
   * and linear indices of elements stays the same but the shape vector is
   * changed.
   */
  Tensor reshape(unsigned int rank, const std::vector<int> &dims) const;

  /**
   * Returns the slices of this tensor along the first axis. This is the inverse
   * of the stack operation.
   */
  std::vector<Tensor> slices() const;

  /**
   * Combines a vector of tensors into a new tensor whose first dimension is the
   * size of the vector. This is the inverse of the slices operation.
   */
  static Tensor stack(const std::vector<Tensor> &slices);

  /**
   * Creates a new tensor from the given tensor whose entries are negated.
   */
  Tensor logic_not() const;

  /**
   * Creates a new tensor from the given pair of tensors whose entries are the
   * logical and of the corresponding literals.
   */
  Tensor logic_and(const Tensor &tensor2) const {
    return logic_bin(&Logic::logic_and, tensor2);
  }

  /**
   * Creates a new tensor from the given pair of tensors whose entries are the
   * logical or of the corresponding literals.
   */
  Tensor logic_or(const Tensor &tensor2) const {
    return logic_bin(&Logic::logic_or, tensor2);
  }

  /**
   * Creates a new tensor from the given pair of tensors whose entries are the
   * implication of the corresponding literals.
   */
  Tensor logic_leq(const Tensor &tensor2) const {
    return logic_bin(&Logic::logic_leq, tensor2);
  }

  /**
   * Creates a new tensor from the given pair of tensors whose entries are the
   * logical sum of the corresponding literals.
   */
  Tensor logic_add(const Tensor &tensor2) const {
    return logic_bin(&Logic::logic_add, tensor2);
  }

  /**
   * Creates a new tensor from the given pair of tensors whose entries are the
   * logical xor of the corresponding literals.
   */
  Tensor logic_equ(const Tensor &tensor2) const {
    return logic_bin(&Logic::logic_equ, tensor2);
  }

  /**
   * Returns a new tensor from the given three tensors whose entries are the
   * logical majority of the corresponding literals.
   */
  Tensor logic_maj(const Tensor &tensor2, const Tensor &tensor3) const {
    return logic_ter(&Logic::logic_maj, tensor2, tensor3);
  }

  /**
   * Returns a new tensor from the given three tensors whose entries are the
   * logical ternary iff of the corresponding literals.
   */
  Tensor logic_iff(const Tensor &tensor2, const Tensor &tensor3) const {
    return logic_ter(&Logic::logic_iff, tensor2, tensor3);
  }

  /**
   * Folds this tensor along the first axis using the logical and operation.
   */
  Tensor fold_all() const;

  /**
   * Folds this tensor along the first axis using the binary or operation.
   */
  Tensor fold_any() const;

  /**
   * Folds this tensor along the first axis using the binary addition operation.
   */
  Tensor fold_sum() const;

  /**
   * Folds this tensor along the first axis and returns true if there is exactly
   * one true value among the folded values.
   */
  Tensor fold_one() const;

  /**
   * Returns the scalar value of a zero rank tensor.
   */
  literal_t get_scalar() const;

  /**
   * Returns the boolean tensor that contains the values of this tensor in the
   * current solution.
   */
  Tensor get_solution(const std::shared_ptr<Solver> &solver) const;

  /**
   * Adds the literals of this tensor to the clause.
   */
  void extend_clause(std::vector<literal_t> &clause) const;

  /**
   * Prints out the tensor content without shape information.
   */
  friend std::ostream &operator<<(std::ostream &out, const Tensor &tensor);
};

} // namespace uasat

#endif // UASAT_TENSOR_HPP
