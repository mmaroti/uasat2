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

#ifndef UASAT_FUNC_HPP
#define UASAT_FUNC_HPP

#include "tensor.hpp"

namespace uasat {

class Function {
private:
  std::vector<int> domain;
  std::vector<int> codomain;

public:
  Function(const std::vector<int> &domain, const std::vector<int> &codomain)
      : domain(domain), codomain(codomain) {}

  const std::vector<int> &get_domain() const { return domain; }
  const std::vector<int> &get_codomain() const { return codomain; }

  bool check_domain(const Tensor &tensor);
  bool check_codomain(const Tensor &tensor);

  virtual Tensor apply(const Tensor &tensor) = 0;
};

} // namespace uasat

#endif // UASAT_FUNC_HPP
