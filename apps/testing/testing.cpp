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

#include <chrono>
#include <iostream>

#include "uasat/tensor.hpp"

int main() {
  std::shared_ptr<uasat::Solver> solver = uasat::Solver::create();

  std::shared_ptr<const uasat::Tensor> tensor1 =
      uasat::Tensor::variable(solver, {2, 2, 3});
  std::cout << tensor1 << std::endl;

  std::shared_ptr<const uasat::Tensor> tensor2 = tensor1->logic_not();
  std::cout << tensor2 << std::endl;

  std::shared_ptr<const uasat::Tensor> tensor3 =
      tensor2->permute({3, 2, 2}, {1, 1, 0});
  std::cout << tensor3 << std::endl;

  std::shared_ptr<const uasat::Tensor> tensor4 = tensor1->logic_add(tensor2);
  std::cout << tensor4 << std::endl;

  return 0;
}
