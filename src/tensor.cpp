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

class IndexView {
private:
  typedef int index_t;

  index_t base_index = 0;
  index_t view_index = 0;
  index_t view_size = 1;

  struct dimension_t {
    dimension_t(index_t size, index_t stride, index_t index)
        : size(size), stride(stride), index(index) {}
    index_t size;
    index_t stride;
    index_t index;
  };

  std::vector<dimension_t> dimensions;

public:
  void add_dimension(index_t size, index_t stride) {
    assert(size > 0);
    if (!dimensions.empty() &&
        dimensions.back().size * dimensions.back().stride == stride) {
      dimensions.back().size *= size;
    } else {
      dimensions.emplace_back(size, stride, 0);
    }
    assert(view_size <= std::numeric_limits<index_t>::max() / size);
    view_size *= size;
  }

  void reset() {
    base_index = 0;
    view_index = 0;
    for (dimension_t &dim : dimensions)
      dim.index = 0;
  }

  bool increment() {
    view_index += 1;
    for (dimension_t &dim : dimensions) {
      base_index += dim.stride;
      if (++dim.index < dim.size)
        return true;
      dim.index = 0;
      base_index -= dim.stride * dim.size;
    }
    assert(base_index == 0);
    assert(view_index == view_size);
    view_index = 0;
    return false;
  }

  index_t get_base_index() const { return base_index; }
  index_t get_view_index() const { return view_index; }
  index_t get_view_size() const { return view_size; }
};

std::size_t get_storage_size(const std::vector<Tensor::index_t> &shape) {
  assert(sizeof(Tensor::index_t) <= sizeof(std::size_t));
  Tensor::index_t size = 1;
  for (Tensor::index_t dim : shape) {
    if (dim <= 0)
      throw std::invalid_argument("dimensions must be positive");
    if (size > std::numeric_limits<Tensor::index_t>::max() / dim)
      throw std::invalid_argument("tensor size is too big");
    size *= dim;
  }
  return size;
}

Tensor::Tensor(const std::shared_ptr<Logic> &logic,
               const std::vector<int> &shape)
    : logic(logic), shape(shape), storage(get_storage_size(shape)) {}

std::unique_ptr<const Tensor>
Tensor::variable(const std::shared_ptr<Solver> &solver,
                 const std::vector<int> &shape, bool decision) {
  std::unique_ptr<Tensor> new_tensor(new Tensor(solver, shape));

  std::vector<int> &new_storage = new_tensor->storage;
  for (size_t i = 0; i < new_storage.size(); i++)
    new_storage[i] = solver->add_variable(decision);

  return new_tensor;
}

std::unique_ptr<const Tensor>
Tensor::constant(const std::shared_ptr<Logic> &logic,
                 const std::vector<int> &shape, literal_t literal) {
  std::unique_ptr<Tensor> new_tensor(new Tensor(logic, shape));

  std::vector<int> &new_storage = new_tensor->storage;
  for (size_t i = 0; i < new_storage.size(); i++)
    new_storage[i] = literal;

  return new_tensor;
}

std::unique_ptr<const Tensor> Tensor::permute(const Tensor *old_tensor,
                                              const std::vector<int> &new_shape,
                                              const std::vector<int> &mapping) {
  const std::vector<int> &old_shape = old_tensor->shape;
  const std::vector<literal_t> &old_storage = old_tensor->storage;

  if (old_shape.size() != mapping.size())
    throw std::invalid_argument("invalid coordinate mapping size");

  std::unique_ptr<Tensor> new_tensor(new Tensor(old_tensor->logic, new_shape));
  std::vector<literal_t> &new_storage = new_tensor->storage;

  int length = 1;
  std::vector<int> stride(new_shape.size(), 0);
  for (size_t i = 0; i < old_shape.size(); i++) {
    if (mapping[i] < 0 || (size_t)mapping[i] > new_shape.size())
      throw std::invalid_argument("invalid coordinate mapping index");
    if (old_shape[i] != new_shape[mapping[i]])
      throw std::invalid_argument("invalid coordinate mapping value");

    stride[mapping[i]] += length;
    length *= old_shape[i];
  }

  std::vector<int> new_coord(new_shape.size(), 0);
  size_t old_index = 0;
  for (size_t new_index = 0; new_index < new_storage.size(); new_index++) {
    new_storage[new_index] = old_storage[old_index];
    for (size_t i = 0; i < new_coord.size();) {
      old_index += stride[i];
      if (++new_coord[i] < new_shape[i])
        break;
      old_index -= stride[i] * new_shape[i];
      new_coord[i++] = 0;
    }
  }

  for (size_t i = 0; i < new_coord.size(); i++)
    assert(new_coord[i] == 0);

  return new_tensor;
}

std::unique_ptr<const Tensor> Tensor::logic_not(const Tensor *tensor) {
  std::unique_ptr<Tensor> new_tensor(new Tensor(tensor->logic, tensor->shape));

  const std::vector<int> &storage = tensor->storage;
  std::vector<int> &new_storage = new_tensor->storage;
  Logic *logic = tensor->logic.get();

  for (size_t i = 0; i < new_storage.size(); i++)
    new_storage[i] = logic->logic_not(storage[i]);

  return new_tensor;
}

std::unique_ptr<const Tensor>
Tensor::logic_bin(literal_t (Logic::*op)(literal_t, literal_t),
                  const Tensor *tensor1, const Tensor *tensor2) {
  if (tensor1->logic != tensor2->logic)
    throw std::invalid_argument("non-matching logic");
  if (tensor1->shape != tensor2->shape)
    throw std::invalid_argument("non-matching shape");

  std::unique_ptr<Tensor> new_tensor(
      new Tensor(tensor1->logic, tensor1->shape));

  const std::vector<int> &storage1 = tensor1->storage;
  const std::vector<int> &storage2 = tensor2->storage;
  std::vector<int> &new_storage = new_tensor->storage;
  Logic *logic = tensor1->logic.get();

  for (size_t i = 0; i < new_storage.size(); i++)
    new_storage[i] = (logic->*op)(storage1[i], storage2[i]);

  return new_tensor;
}

std::unique_ptr<const Tensor>
Tensor::fold_bin(literal_t (Logic::*op)(const std::vector<literal_t> &),
                 const Tensor *tensor, const std::vector<bool> &selection) {
  if (selection.size() != tensor->shape.size())
    throw std::invalid_argument("invalid fold selection");

  std::vector<index_t> scan_shape;
  IndexView fold_view;
  IndexView scan_view;
  index_t length = 1;
  for (std::size_t i = 0; i < selection.size(); i++) {
    if (selection[i])
      fold_view.add_dimension(tensor->shape[i], length);
    else {
      scan_view.add_dimension(tensor->shape[i], length);
      scan_shape.emplace_back(tensor->shape[i]);
    }
    length *= tensor->shape[i];
  }

  Logic *logic = tensor->logic.get();
  const std::vector<literal_t> &storage = tensor->storage;
  std::unique_ptr<Tensor> scan_tensor(new Tensor(tensor->logic, scan_shape));
  std::vector<literal_t> &scan_storage = scan_tensor->storage;
  assert(scan_storage.size() == (std::size_t)scan_view.get_view_size());
  std::vector<literal_t> fold_storage(fold_view.get_view_size());

  do {
    index_t scan_base_index = scan_view.get_base_index();
    do {
      fold_storage[fold_view.get_view_index()] =
          storage[scan_base_index + fold_view.get_base_index()];
    } while (fold_view.increment());
    scan_storage[scan_view.get_view_index()] = (logic->*op)(fold_storage);
  } while (scan_view.increment());

  return scan_tensor;
}

std::ostream &operator<<(std::ostream &out, const Tensor *tensor) {
  const std::vector<literal_t> &storage = tensor->storage;

  out << '[';
  for (std::size_t i = 0; i < storage.size(); i++) {
    if (i != 0)
      out << ',';
    out << storage[i];
  }
  out << ']';
  return out;
}

} // namespace uasat
