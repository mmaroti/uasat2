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

#include "uasat/clone.hpp"
#include <cassert>
#include <stdexcept>

namespace uasat {

Tensor OperationalClone::compose(const Tensor &func, int arity2,
                                 const Tensor &arg0) {
  return compose(1, func, arity2, Tensor::stack({arg0}));
}

Tensor OperationalClone::compose(const Tensor &func, int arity2,
                                 const Tensor &arg0, const Tensor &arg1) {
  return compose(2, func, arity2, Tensor::stack({arg0, arg1}));
}

Tensor OperationalClone::compose(const Tensor &func, int arity2,
                                 const Tensor &arg0, const Tensor &arg1,
                                 const Tensor &arg2) {
  return compose(3, func, arity2, Tensor::stack({arg0, arg1, arg2}));
}

Operations::Operations(int size) : size(size) {
  if (size <= 0)
    throw std::invalid_argument("size must be positive");
}

std::vector<int> Operations::get_shape(int arity) const {
  assert(arity >= 0);

  return std::vector<int>(arity + 1, size);
}

Tensor Operations::contains(int arity, const Tensor &elem) {
  assert(arity >= 0);
  assert(elem.get_shape().size() >= 1 + size_t(arity));

  Tensor result = elem.fold_one();
  for (int i = 0; i < arity; i++)
    result = result.fold_all();

  return result;
}

Tensor Operations::projection(int arity, int index) {
  assert(0 <= index && index < arity);

  std::vector<int> shape(arity + 1, 1);
  shape[0] = size;
  shape[1 + index] = size;

  return Tensor::diagonal(size).reshape(2, shape);
}

Tensor Operations::compose(int arity1, const Tensor &func, int arity2,
                           const Tensor &args) {
  assert(!args.get_shape().empty() && arity1 == args.get_shape()[0]);
  assert(arity1 >= 0 && func.get_shape().size() >= 1 + size_t(arity1));
  assert(arity2 >= 0 && args.get_shape().size() >= 2 + size_t(arity2));

  // TODO: implement it properly
  return projection(1, 0);
}

} // namespace uasat
