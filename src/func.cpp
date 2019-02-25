/*
 * Copyright (c) 2016-2019, Miklos Maroti, University of Szeged
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

#include "uasat/func.hpp"
#include <stdexcept>

namespace uasat {

bool check_shape(const std::vector<int> &shape, const Tensor &elem) {
  const std::vector<int> &shape2 = elem.get_shape();
  if (shape2.size() < shape.size())
    return false;

  for (size_t i = 0; i < shape.size(); i++)
    if (shape[i] != shape2[i])
      return false;

  return true;
}

Constant::Constant(const shape_t &domain, bool value)
    : domain(domain), tensor(Tensor::constant(domain, value)) {}

class Identity : public UnaryFunc {
protected:
  const shape_t domain;

public:
  Identity(const shape_t &domain) : domain(domain) {}

  const shape_t &get_domain() const override { return domain; }
  const shape_t &get_codomain() const override { return domain; }

  Tensor apply(const Tensor &tensor) const override {
    assert(domain == tensor.get_shape());
    return tensor;
  }
};

std::unique_ptr<UnaryFunc> identity(const shape_t &domain) {
  return std::make_unique<Identity>(domain);
}

class Compose11 : public UnaryFunc {
protected:
  const std::unique_ptr<UnaryFunc> func;
  const std::unique_ptr<UnaryFunc> arg;

public:
  Compose11(std::unique_ptr<UnaryFunc> func, std::unique_ptr<UnaryFunc> arg)
      : func(std::move(func)), arg(std::move(arg)) {}

  const shape_t &get_domain() const override { return arg->get_domain(); }
  const shape_t &get_codomain() const override { return func->get_codomain(); }

  Tensor apply(const Tensor &tensor) const override {
    return func->apply(arg->apply(tensor));
  }
};

std::unique_ptr<UnaryFunc> compose(std::unique_ptr<UnaryFunc> func,
                                   std::unique_ptr<UnaryFunc> arg) {
  return std::make_unique<Compose11>(std::move(func), std::move(arg));
}

} // namespace uasat
