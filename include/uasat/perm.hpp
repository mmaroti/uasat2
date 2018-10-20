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

#ifndef UASAT_CLASSES_HPP
#define UASAT_CLASSES_HPP

#include <vector>

#include "tensor.hpp"

namespace uasat {

class Group {
public:
  /**
   * The elements of the group are tensors of this shape.
   */
  virtual std::vector<int> get_shape() const = 0;

  /**
   * Calculates the membership relation, that is whether the given tensor
   * element is a member of this group.
   */
  virtual Tensor contains(const Tensor &elem) = 0;

  /**
   * Calculates the identity element of the group.
   */
  virtual Tensor identity() = 0;

  /**
   * Calculates the inverse of the given element.
   */
  virtual Tensor inverse(const Tensor &elem) = 0;

  /**
   * Calculates the product of the given pair of elements.
   */
  virtual Tensor product(const Tensor &elem1, const Tensor &elem2) = 0;

  /**
   * Tests that the underlying set is closed under the operations and the
   * operations satisfy the group axioms.
   */
  void test_axioms();
};

class SymmetricGroup : public virtual Group {
private:
  int size;

public:
  SymmetricGroup(int size);

  std::vector<int> get_shape() const override;
  Tensor contains(const Tensor &elem) override;
  Tensor identity() override;
  Tensor inverse(const Tensor &perm) override;
  Tensor product(const Tensor &perm1, const Tensor &perm2) override;

  Tensor even(const Tensor &perm);
};

} // namespace uasat

#endif // UASAT_CLASSES_HPP
