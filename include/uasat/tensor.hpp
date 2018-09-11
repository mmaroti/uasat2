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

#include "uasat/solver.hpp"

namespace uasat {

class Tensor {
public:
  const std::vector<int> shape;
  const int length;
  const std::shared_ptr<Logic> logic;

protected:
  const std::vector<int> strides;
  std::shared_ptr<std::vector<literal_t>> storage;

public:
  static std::unique_ptr<Tensor>
  create_variables(const std::vector<int> &shape,
                   const std::shared_ptr<Solver> &solver, bool decision = true);

  virtual ~Tensor() = default;

  class iterator {
  protected:
    std::vector<int> indices;
    int offset;

  public:
  };

protected:
  Tensor(const std::vector<int> &shape, const std::shared_ptr<Logic> &logic);
};

} // namespace uasat

#endif // UASAT_TENSOR_HPP
