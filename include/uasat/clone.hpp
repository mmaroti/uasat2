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

#ifndef UASAT_GROUP_HPP
#define UASAT_GROUP_HPP

#include "uasat/set.hpp"

namespace uasat {

class AbstractClone : public GradedSet {
public:
  /**
   * Returns the projection element. The arity must be positive and the index be
   * in the interval [0, arity-1].
   */
  virtual Tensor projection(int arity, int index) = 0;

  /**
   * Calculates the composition of a function and tuple of elements. The
   * dimension of the first axes must match arity1. The result will have arity2.
   */
  virtual Tensor compose(int arity1, const Tensor &func, int arity2,
                         const Tensor &args) = 0;

  /**
   * Composition of a unary function and a single argument.
   */
  Tensor compose(const Tensor &func, int arity2, const Tensor &arg0);

  /**
   * Composition of a binary function and a pair of arguments.
   */
  Tensor compose(const Tensor &func, int arity2, const Tensor &arg0,
                 const Tensor &arg1);

  /**
   * Composition of a ternary function and three arguments.
   */
  Tensor compose(const Tensor &func, int arity2, const Tensor &arg0,
                 const Tensor &arg1, const Tensor &arg2);
};

class Operations : public AbstractClone {
protected:
  int size;

public:
  /**
   * Creates a clone of operations on a finite set of the given size.
   * The size must be positive.
   */
  Operations(int size);

  std::vector<int> get_shape(int arity) const override;
  Tensor contains(int arity, const Tensor &elem) override;
  Tensor projection(int arity, int index) override;
  Tensor compose(int arity1, const Tensor &func, int arity2,
                 const Tensor &args) override;
};

} // namespace uasat

#endif // UASAT_GROUP_HPP
