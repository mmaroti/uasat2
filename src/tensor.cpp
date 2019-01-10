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

#include <cassert>
#include <limits>
#include <stdexcept>

#include "uasat/tensor.hpp"

namespace uasat {

class View {
public:
  size_t offset = 0;
  size_t index = 0;
  size_t size = 1;

  struct dimension_t {
    dimension_t(size_t size, size_t stride)
        : index(0), size(size), stride(stride) {}
    size_t index;
    size_t size;
    size_t stride;
  };

  std::vector<dimension_t> dimensions;

  void add(int dim_size, size_t stride) {
    assert(dim_size > 0);
    if (!dimensions.empty() &&
        dimensions.back().size * dimensions.back().stride == stride) {
      dimensions.back().size *= dim_size;
    } else
      dimensions.emplace_back(dim_size, stride);

    assert(size <= std::numeric_limits<size_t>::max() / dim_size);
    size *= dim_size;
  }

  void reset() {
    offset = 0;
    index = 0;
    for (dimension_t &dim : dimensions)
      dim.index = 0;
  }

  bool next() {
    index += 1;
    for (dimension_t &dim : dimensions) {
      offset += dim.stride;
      if (++dim.index < dim.size)
        return true;
      dim.index = 0;
      offset -= dim.stride * dim.size;
    }
    assert(offset == 0 && index == size);
    index = 0;
    return false;
  }
};

size_t get_storage_size(const std::vector<int> &shape) {
  size_t size = 1;
  for (int dimension : shape) {
    if (dimension <= 0)
      throw std::invalid_argument("dimension must be positive");
    if (size > std::numeric_limits<size_t>::max() / dimension)
      throw std::invalid_argument("tensor size is too big");
    size *= dimension;
  }
  return size;
}

Tensor::Tensor(const std::shared_ptr<Logic> &logic,
               const std::vector<int> &shape)
    : logic(logic), shape(shape), storage(get_storage_size(shape)) {}

std::shared_ptr<Logic> Tensor::join_logic(const Tensor &tensor) const {
  if (logic != tensor.logic && logic != BOOLEAN && tensor.logic != BOOLEAN)
    throw std::invalid_argument("non-matching tensor logics");
  return logic != BOOLEAN ? logic : tensor.logic;
}

size_t
Tensor::__very_slow_get_index(const std::vector<int> &coordinates) const {
  if (coordinates.size() != shape.size())
    throw std::invalid_argument("invalid number of coordinates");

  size_t index = 0;
  size_t size = 1;
  for (size_t axis = 0; axis < coordinates.size(); axis++) {
    if (coordinates[axis] < 0 || coordinates[axis] >= shape[axis])
      throw std::invalid_argument("invalid coordinate value");

    index += coordinates[axis] * size;
    size *= shape[axis];
  }

  assert(size == storage.size());
  assert(index < storage.size());
  return storage[index];
}

Tensor Tensor::variable(const std::shared_ptr<Solver> &solver,
                        const std::vector<int> &shape, bool decision,
                        bool polarity) {
  Tensor tensor(solver, shape);
  for (literal_t &value : tensor.storage)
    value = solver->add_variable(decision, polarity);

  return tensor;
}

Tensor Tensor::constant(const std::vector<int> &shape, bool value) {
  Tensor tensor(BOOLEAN, shape);

  literal_t literal = value ? BOOLEAN->TRUE : BOOLEAN->FALSE;
  for (literal_t &lit : tensor.storage)
    lit = literal;

  return tensor;
}

Tensor Tensor::diagonal(int dimension) {
  Tensor tensor(BOOLEAN, {dimension, dimension});
  for (int i = 0; i < dimension; i++)
    for (int j = 0; j < dimension; j++)
      tensor.storage[i * dimension + j] =
          (i == j) ? BOOLEAN->TRUE : BOOLEAN->FALSE;

  return tensor;
}

Tensor Tensor::lessthan(int dimension) {
  Tensor tensor(BOOLEAN, {dimension, dimension});
  for (int i = 0; i < dimension; i++)
    for (int j = 0; j < dimension; j++)
      tensor.storage[i * dimension + j] =
          (i < j) ? BOOLEAN->TRUE : BOOLEAN->FALSE;

  return tensor;
}

Tensor Tensor::polymer(const std::vector<int> &shape2,
                       const std::vector<int> &mapping) const {

  if (shape.size() != mapping.size())
    throw std::invalid_argument("invalid coordinate mapping size");

  Tensor tensor2(logic, shape2);

  size_t size = 1;
  std::vector<size_t> stride2(shape2.size(), 0);
  for (size_t axis = 0; axis < shape.size(); axis++) {
    if (mapping[axis] < 0 || (size_t)mapping[axis] >= shape2.size())
      throw std::invalid_argument("invalid coordinate mapping index");
    if (shape[axis] != shape2[mapping[axis]])
      throw std::invalid_argument("invalid coordinate mapping value");

    stride2[mapping[axis]] += size;
    size *= shape[axis];
  }

  View view;
  for (size_t axis = 0; axis < shape2.size(); axis++)
    view.add(shape2[axis], stride2[axis]);

  do {
    tensor2.storage[view.index] = storage[view.offset];
  } while (view.next());

  return tensor2;
}

Tensor Tensor::reshape(unsigned int rank, const std::vector<int> &dims) const {
  if (rank < shape.size())
    throw std::invalid_argument("invalid resize rank");

  std::vector<int> shape2(shape.size() - rank + dims.size());
  std::copy(dims.begin(), dims.end(), shape2.begin());
  std::copy(shape.begin() + rank, shape.end(), shape2.begin() + dims.size());

  Tensor tensor2(logic, shape2);
  if (storage.size() != tensor2.storage.size())
    throw std::invalid_argument("invalid resize dims");

  std::copy(storage.begin(), storage.end(), tensor2.storage.begin());
  return tensor2;
}

std::vector<Tensor> Tensor::slices() const {
  if (shape.size() < 1)
    throw std::invalid_argument("not enough tenxor axes");

  size_t size1 = shape[0];
  size_t size2 = storage.size() / size1;
  assert(size1 * size2 == storage.size());

  std::vector<int> shape2(shape.size() - 1);
  std::copy(shape.begin() + 1, shape.end(), shape2.begin());

  std::vector<Tensor> slices;
  slices.reserve(size1);

  for (size_t i = 0; i < size1; i++)
    slices.push_back(Tensor(logic, shape2));

  for (size_t i = 0; i < size2; i++)
    for (size_t j = 0; j < size1; j++)
      slices[j].storage[i] = storage[i * size1 + j];

  return slices;
}

Tensor Tensor::stack(const std::vector<Tensor> &slices) {
  if (slices.empty())
    throw std::invalid_argument("slices list cannot be empty");

  std::vector<int> shape = slices[0].shape;
  std::shared_ptr<Logic> logic = slices[0].logic;

  size_t dim = slices.size();

  for (size_t i = 1; i < dim; i++) {
    if (shape != slices[i].shape)
      throw std::invalid_argument("slices must have same shape");
    logic = Logic::join(logic, slices[i].logic);
  }

  shape.insert(shape.begin(), slices.size());
  Tensor tensor(logic, shape);

  size_t size = slices[0].storage.size();

  for (size_t i = 0; i < size; i++)
    for (size_t j = 0; j < slices.size(); j++)
      tensor.storage[i * size + j] = slices[j].storage[i];

  return tensor;
}

Tensor Tensor::logic_not() const {
  Tensor tensor2(logic, shape);

  for (size_t index = 0; index < tensor2.storage.size(); index++)
    tensor2.storage[index] = logic->logic_not(storage[index]);

  return tensor2;
}

Tensor Tensor::logic_bin(literal_t (Logic::*op)(literal_t, literal_t),
                         const Tensor &tensor2) const {
  if (shape != tensor2.shape)
    throw std::invalid_argument("non-matching shape");

  std::shared_ptr<Logic> logic3 = join_logic(tensor2);
  Tensor tensor3(logic3, shape);

  for (size_t index = 0; index < tensor3.storage.size(); index++)
    tensor3.storage[index] =
        (logic3.get()->*op)(storage[index], tensor2.storage[index]);

  return tensor3;
}

Tensor Tensor::fold_all() const {
  std::vector<Tensor> tensors = slices();
  assert(tensors.size() > 0);

  Tensor data = tensors[0];
  for (size_t i = 1; i < tensors.size(); i++)
    data = data.logic_and(tensors[i]);

  return data;
}

Tensor Tensor::fold_any() const {
  std::vector<Tensor> tensors = slices();
  assert(tensors.size() > 0);

  Tensor data = tensors[0];
  for (size_t i = 1; i < tensors.size(); i++)
    data = data.logic_or(tensors[i]);

  return data;
}

Tensor Tensor::fold_sum() const {
  std::vector<Tensor> tensors = slices();
  assert(tensors.size() > 0);

  Tensor data = tensors[0];
  for (size_t i = 1; i < tensors.size(); i++)
    data = data.logic_add(tensors[i]);

  return data;
}

Tensor Tensor::fold_one() const {
  std::vector<Tensor> tensors = slices();
  assert(tensors.size() > 0);

  if (tensors.size() == 1)
    return tensors[0];

  Tensor min1 = tensors[0].logic_or(tensors[1]);
  Tensor min2 = tensors[0].logic_and(tensors[1]);
  for (size_t i = 2; i < tensors.size(); i++) {
    min2 = min2.logic_or(min1.logic_and(tensors[i]));
    min1 = min1.logic_or(tensors[i]);
  }

  return min1.logic_and(min2.logic_not());
}

literal_t Tensor::get_scalar() const {
  if (storage.size() != 1)
    throw std::invalid_argument("tensor must be scalar");
  return storage[0];
}

Tensor Tensor::get_solution(const std::shared_ptr<Solver> &solver) const {
  if (solver.get() != logic.get())
    throw std::invalid_argument("non-matching solver");

  Tensor tensor(BOOLEAN, shape);
  for (size_t index = 0; index < tensor.storage.size(); index++)
    tensor.storage[index] = solver->get_solution(storage[index]);

  return tensor;
}

void Tensor::extend_clause(std::vector<literal_t> &clause) const {
  for (size_t index = 0; index < storage.size(); index++)
    clause.push_back(storage[index]);
}

std::ostream &operator<<(std::ostream &out, const Tensor &tensor) {
  out << '[';
  for (size_t index = 0; index < tensor.storage.size(); index++) {
    if (index != 0)
      out << ',';
    out << tensor.storage[index];
  }
  out << ']';
  return out;
}

} // namespace uasat
