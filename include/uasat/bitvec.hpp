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

#ifndef UASAT_BITVEC_HPP
#define UASAT_BITVEC_HPP

#include "set.hpp"

namespace uasat {

class BitVector : public AbstractSet {
protected:
  int length;

public:
  BitVector(int length);

  virtual Tensor contains(const Tensor &elem) override;
  Tensor constant(const std::vector<int> &shape, unsigned long value);

  /**
   * Returns the intersection of the two bit vectors, which is their bitwise
   * logical and.
   */
  Tensor meet(const Tensor &elem1, const Tensor &elem2);

  /**
   * Returns the union of the two bit vectors, which is their bitwise logical
   * or.
   */
  Tensor join(const Tensor &elem1, const Tensor &elem2);

  /**
   * Returns true if the first bit vector is a subset of the second one.
   */
  Tensor less(const Tensor &elem1, const Tensor &elem2);

  /**
   * Returns the binary number that is the sum of the binary number elem and
   * bit.
   */
  Tensor plus_one(const Tensor &elem, const Tensor &bit);

  /**
   * Returns the number of bits set to one in the element.
   */
  Tensor weight(const Tensor &elem);
};

} // namespace uasat

#endif // UASAT_BITVEC_HPP
