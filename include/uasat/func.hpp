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

#include <memory>

namespace uasat {

class Tensor;
class shape_t;

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

std::unique_ptr<NullaryFunc> constant(const shape_t &domain, bool value);

std::unique_ptr<NullaryFunc> compose(std::unique_ptr<UnaryFunc> func,
                                     std::unique_ptr<NullaryFunc> arg);

std::unique_ptr<NullaryFunc> compose(std::unique_ptr<BinaryFunc> func,
                                     std::unique_ptr<NullaryFunc> arg1,
                                     std::unique_ptr<NullaryFunc> arg2);

std::unique_ptr<UnaryFunc> identity(const shape_t &domain);

std::unique_ptr<UnaryFunc> logic_not(const shape_t &domain);

std::unique_ptr<UnaryFunc> compose(std::unique_ptr<UnaryFunc> func,
                                   std::unique_ptr<UnaryFunc> arg);

std::unique_ptr<BinaryFunc> logic_and(const shape_t &domain);

std::unique_ptr<BinaryFunc> logic_or(const shape_t &domain);

std::unique_ptr<BinaryFunc> logic_equ(const shape_t &domain);

std::unique_ptr<BinaryFunc> logic_leq(const shape_t &domain);

std::unique_ptr<BinaryFunc> logic_add(const shape_t &domain);

} // namespace uasat

#endif // UASAT_FUNC_HPP
