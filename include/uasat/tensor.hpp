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
public:
  const std::shared_ptr<Logic> logic;
  const std::vector<int> shape;

protected:
  std::vector<literal_t> storage;

  Tensor(const std::shared_ptr<Logic> &logic, const std::vector<int> &shape);

  /**
   * Performs the given generic binary logic operation on the two tensors.
   */
  static std::unique_ptr<const Tensor>
  logic_bin(literal_t (Logic::*op)(literal_t, literal_t), const Tensor *tensor1,
            const Tensor *tensor2);

  /**
   * Performs the given generic fold operation on the given tensor,
   * at the selected coordinate.
   */
  static std::unique_ptr<const Tensor>
  fold_bin(literal_t (Logic::*op)(const std::vector<literal_t> &),
           const Tensor *tensor, const std::vector<bool> &selection);

public:
  /**
   * Creates a new tensor with the given shape with fresh variables from
   * the selected solver.
   */
  static std::unique_ptr<const Tensor>
  variable(const std::shared_ptr<Solver> &solver, const std::vector<int> &shape,
           bool decision = true);

  /**
   * Creates a new tensor with the given shape filled with the same literal.
   */
  static std::unique_ptr<const Tensor>
  constant(const std::shared_ptr<Logic> &logic, const std::vector<int> &shape,
           literal_t literal);

  /**
   * Creates a new tensor of the given shape from the given old tensor with
   * permuted, identified or new coordinates. The mapping is a vector
   * of length of the old tensor shape with entries identifing the coordinate
   * in the new tensor.
   */
  static std::unique_ptr<const Tensor> permute(const Tensor *tensor,
                                               const std::vector<int> &shape,
                                               const std::vector<int> &mapping);

  std::unique_ptr<const Tensor> permute(const std::vector<int> &shape,
                                        const std::vector<int> &mapping) const {
    return Tensor::permute(this, shape, mapping);
  }

  /**
   * Creates a new tensor from the given tensor whose entries are negated.
   */
  static std::unique_ptr<const Tensor> logic_not(const Tensor *tensor);

  static std::unique_ptr<const Tensor>
  logic_not(const std::unique_ptr<const Tensor> &tensor) {
    return Tensor::logic_not(tensor.get());
  }

  std::unique_ptr<const Tensor> logic_not() const {
    return Tensor::logic_not(this);
  }

  /**
   * Creates a new tensor from the given pair of tensors whose entries are
   * the logical and of the corresponding literals.
   */
  static std::unique_ptr<const Tensor> logic_and(const Tensor *tensor1,
                                                 const Tensor *tensor2) {
    return Tensor::logic_bin(&Logic::logic_and, tensor1, tensor2);
  }

  static std::unique_ptr<const Tensor>
  logic_and(const std::unique_ptr<const Tensor> &tensor1,
            const std::unique_ptr<const Tensor> &tensor2) {
    return Tensor::logic_and(tensor1.get(), tensor2.get());
  }

  std::unique_ptr<const Tensor>
  logic_and(const std::unique_ptr<const Tensor> &tensor2) const {
    return Tensor::logic_and(this, tensor2.get());
  }

  /**
   * Creates a new tensor from the given pair of tensors whose entries are
   * the logical or of the corresponding literals.
   */
  static std::unique_ptr<const Tensor> logic_or(const Tensor *tensor1,
                                                const Tensor *tensor2) {
    return Tensor::logic_bin(&Logic::logic_or, tensor1, tensor2);
  }

  static std::unique_ptr<const Tensor>
  logic_or(const std::unique_ptr<const Tensor> &tensor1,
           const std::unique_ptr<const Tensor> &tensor2) {
    return Tensor::logic_or(tensor1.get(), tensor2.get());
  }

  std::unique_ptr<const Tensor>
  logic_or(const std::unique_ptr<const Tensor> &tensor2) const {
    return Tensor::logic_or(this, tensor2.get());
  }

  /**
   * Creates a new tensor from the given pair of tensors whose entries are
   * the logical sum of the corresponding literals.
   */
  static std::unique_ptr<const Tensor> logic_add(const Tensor *tensor1,
                                                 const Tensor *tensor2) {
    return Tensor::logic_bin(&Logic::logic_add, tensor1, tensor2);
  }

  static std::unique_ptr<const Tensor>
  logic_add(const std::unique_ptr<const Tensor> &tensor1,
            const std::unique_ptr<const Tensor> &tensor2) {
    return Tensor::logic_add(tensor1.get(), tensor2.get());
  }

  std::unique_ptr<const Tensor>
  logic_add(const std::unique_ptr<const Tensor> &tensor2) const {
    return Tensor::logic_add(this, tensor2.get());
  }

  /**
   * Creates a new tensor from the given pair of tensors whose entries are
   * the logical xor of the corresponding literals.
   */
  static std::unique_ptr<const Tensor> logic_xor(const Tensor *tensor1,
                                                 const Tensor *tensor2) {
    return Tensor::logic_bin(&Logic::logic_xor, tensor1, tensor2);
  }

  static std::unique_ptr<const Tensor>
  logic_xor(const std::unique_ptr<const Tensor> &tensor1,
            const std::unique_ptr<const Tensor> &tensor2) {
    return Tensor::logic_xor(tensor1.get(), tensor2.get());
  }

  std::unique_ptr<const Tensor>
  logic_xor(const std::unique_ptr<const Tensor> &tensor2) const {
    return Tensor::logic_xor(this, tensor2.get());
  }

  /**
   * Prints out the tensor content without shape information.
   */
  friend std::ostream &operator<<(std::ostream &out, const Tensor *tensor);
};

inline std::ostream &operator<<(std::ostream &out,
                                const std::unique_ptr<const Tensor> &tensor) {
  return (out << tensor.get());
}

} // namespace uasat

#endif // UASAT_TENSOR_HPP
