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

#include "uasat/bitvec.hpp"
#include <cassert>
#include <limits>
#include <stdexcept>

namespace uasat {

BitVector::BitVector(int length) : AbstractSet({length}), length(length) {
  if (length < 0)
    throw std::invalid_argument("number of bits cannot be negative");
}

Tensor BitVector::contains(const Tensor &elem) {
  const std::vector<int> &shape = elem.get_shape();
  assert(shape.size() > 0 && shape[0] == length);

  std::vector<int> shape2(shape.size() - 1, 0);
  std::copy(shape.begin() + 1, shape.end(), shape2.begin());

  return Tensor::constant(shape2, true);
}

Tensor BitVector::constant(unsigned long value) {
  assert(value <= std::numeric_limits<unsigned long>::max());
  assert((value >> length) == 0);

  std::vector<Tensor> bits;
  for (int i = 0; i < length; i++) {
    bool b = ((value >> i) & 1) != 0;
    bits.push_back(Tensor::constant({}, b));
  }

  return Tensor::stack(bits);
}

Tensor BitVector::plus_one(const Tensor &elem, const Tensor &bit) {
  std::vector<Tensor> bits = elem.slices();
  assert(bits.size() > 0);

  std::vector<Tensor> result;

  Tensor carry = bit;
  for (size_t i = 0; i < bits.size(); i++) {
    result.push_back(bits[i].logic_add(carry));
    carry = bits[i].logic_and(carry);
  }

  return Tensor::stack(result);
}

} // namespace uasat
