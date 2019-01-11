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

#ifndef UASAT_SET_HPP
#define UASAT_SET_HPP

#include "tensor.hpp"
#include <vector>

namespace uasat {

class Set {
protected:
  std::vector<int> shape;
  int tensor_size;

public:
  /**
   * Initializes the shape of the elements of this set.
   */
  Set(const std::vector<int> &shape);

  /**
   * The elements of the set are tensors of this shape.
   */
  const std::vector<int> &get_shape() const { return shape; };

  /**
   * Calculates the membership relation, that is whether the given tensor
   * is a member of this set.
   */
  virtual Tensor contains(const Tensor &elem) = 0;

  /**
   * Tests if the two tensors represent the same element. Note, that is
   * the equality operation on the tensors since otherwise it is hard to
   * list all elements (exclude elements already found).
   */
  Tensor equals(const Tensor &elem1, const Tensor &elem2);

  /**
   * Calculates all elements of this set and puts them into a single tensor
   * whose first axis is the element index.
   */
  Tensor find_elements();

  /**
   * Returns the cardinality of this set.
   */
  int find_cardinality();
};

} // namespace uasat

#endif // UASAT_GROUP_HPP
