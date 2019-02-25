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

#ifndef UASAT_FUNC_HPP
#define UASAT_FUNC_HPP

#include "shape.hpp"
#include "tensor.hpp"

namespace uasat {

class NullaryFunc {
public:
  virtual const shape_t &get_codomain() const = 0;

  virtual const Tensor &apply() const = 0;
};

class UnaryFunc {
public:
  virtual const shape_t &get_domain() const = 0;
  virtual const shape_t &get_codomain() const = 0;

  virtual Tensor apply(const Tensor &elem) const = 0;
};

class BinaryFunc {
public:
  virtual const shape_t &get_domain1() const = 0;
  virtual const shape_t &get_domain2() const = 0;
  virtual const shape_t &get_codomain() const = 0;

  virtual Tensor apply(const Tensor &elem1, const Tensor &elem2) const = 0;
};

std::unique_ptr<UnaryFunc> identity(const shape_t &domain);

std::unique_ptr<NullaryFunc> compose(std::unique_ptr<UnaryFunc> func,
                                     std::unique_ptr<NullaryFunc> arg);

std::unique_ptr<UnaryFunc> compose(std::unique_ptr<UnaryFunc> func,
                                   std::unique_ptr<UnaryFunc> arg);

class Constant : NullaryFunc {
protected:
  const shape_t domain;
  const Tensor tensor;

public:
  Constant(const shape_t &domain, bool value);

  const shape_t &get_codomain() const override { return domain; }

  const Tensor &apply() const override { return tensor; }
};

class LogicNot : UnaryFunc {
protected:
  const shape_t domain;

public:
  LogicNot(const shape_t &domain) : domain(domain) {}

  const shape_t &get_domain() const override { return domain; }
  const shape_t &get_codomain() const override { return domain; }

  Tensor apply(const Tensor &tensor) const override {
    assert(domain == tensor.get_shape());
    return tensor.logic_not();
  }
};

class LogicAnd : BinaryFunc {
protected:
  const shape_t domain;

public:
  LogicAnd(const shape_t &domain) : domain(domain) {}

  const shape_t &get_domain1() const override { return domain; }
  const shape_t &get_domain2() const override { return domain; }
  const shape_t &get_codomain() const override { return domain; }

  Tensor apply(const Tensor &tensor1, const Tensor &tensor2) const override {
    assert(domain == tensor1.get_shape());
    assert(domain == tensor2.get_shape());
    return tensor1.logic_and(tensor2);
  }
};

class LogicOr : BinaryFunc {
protected:
  const shape_t domain;

public:
  LogicOr(const shape_t &domain) : domain(domain) {}

  const shape_t &get_domain1() const override { return domain; }
  const shape_t &get_domain2() const override { return domain; }
  const shape_t &get_codomain() const override { return domain; }

  Tensor apply(const Tensor &tensor1, const Tensor &tensor2) const override {
    assert(domain == tensor1.get_shape());
    assert(domain == tensor2.get_shape());
    return tensor1.logic_or(tensor2);
  }
};

class LogicEqu : BinaryFunc {
protected:
  const shape_t domain;

public:
  LogicEqu(const shape_t &domain) : domain(domain) {}

  const shape_t &get_domain1() const override { return domain; }
  const shape_t &get_domain2() const override { return domain; }
  const shape_t &get_codomain() const override { return domain; }

  Tensor apply(const Tensor &tensor1, const Tensor &tensor2) const override {
    assert(domain == tensor1.get_shape());
    assert(domain == tensor2.get_shape());
    return tensor1.logic_equ(tensor2);
  }
};

class LogicLeq : BinaryFunc {
protected:
  const shape_t domain;

public:
  LogicLeq(const shape_t &domain) : domain(domain) {}

  const shape_t &get_domain1() const override { return domain; }
  const shape_t &get_domain2() const override { return domain; }
  const shape_t &get_codomain() const override { return domain; }

  Tensor apply(const Tensor &tensor1, const Tensor &tensor2) const override {
    assert(domain == tensor1.get_shape());
    assert(domain == tensor2.get_shape());
    return tensor1.logic_leq(tensor2);
  }
};

class LogicAdd : BinaryFunc {
protected:
  const shape_t domain;

public:
  LogicAdd(const shape_t &domain) : domain(domain) {}

  const shape_t &get_domain1() const override { return domain; }
  const shape_t &get_domain2() const override { return domain; }
  const shape_t &get_codomain() const override { return domain; }

  Tensor apply(const Tensor &tensor1, const Tensor &tensor2) const override {
    assert(domain == tensor1.get_shape());
    assert(domain == tensor2.get_shape());
    return tensor1.logic_add(tensor2);
  }
};

} // namespace uasat

#endif // UASAT_FUNC_HPP
