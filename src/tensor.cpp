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

#include <limits>
#include <stdexcept>

#include "uasat/tensor.hpp"

namespace uasat {

int get_initial_length(const std::vector<int> &shape) {
  long length = 1;
  for (int dim : shape) {
    if (dim < 0)
      throw new std::invalid_argument("dimensions must be non-negative");
    length *= dim;
    if (length > std::numeric_limits<int>::max())
      throw new std::invalid_argument("tensor length is too big");
  }
  return length;
}

std::vector<int> get_initial_strides(const std::vector<int> &shape) {
  std::vector<int> strides(shape.size());
  int length = 1;
  for (size_t i = 0; i < shape.size(); i++) {
    strides[i] = length;
    length *= shape[i];
  }
  return strides;
}

Tensor::Tensor(const std::vector<int> &shape,
               const std::shared_ptr<Logic> &logic)
    : shape(shape), length(get_initial_length(shape)), logic(logic),
      strides(get_initial_strides(shape)),
      storage(std::make_shared<std::vector<literal_t>>(length)) {}

std::unique_ptr<Tensor>
Tensor::create_variables(const std::vector<int> &shape,
                         const std::shared_ptr<Solver> &solver, bool decision) {
  std::unique_ptr<Tensor> tensor(new Tensor(shape, solver));

  size_t size = tensor->storage->size();
  literal_t *elements = tensor->storage->data();
  for (size_t i = 0; i < size; i++)
    elements[i] = solver->add_variable(decision);

  return tensor;
}

} // namespace uasat
