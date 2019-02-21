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

#include "uasat/group.hpp"
#include "uasat/shape.hpp"

void test_group() {
  // uasat::SymmetricGroup g(4);
  uasat::BinaryNumAddition g(5);

  g.test_axioms();
  std::cout << "cardinality: " << g.find_cardinality() << std::endl;
}

void test_logic() {
  uasat::Tensor a = uasat::Tensor::lessthan(4);

  for (uasat::Tensor b : a.slices())
    std::cout << b << std::endl;

  std::cout << uasat::Tensor::stack(a.slices()).logic_add(a) << std::endl;
}

void test_binarynum() {
  uasat::BinaryNumAddition v(5);

  if (false) {
    for (int i = 0; i < 32; i++)
      std::cout << v.constant(i) << std::endl;
  }

  std::cout << v.increment(v.constant(5), uasat::Tensor::constant({}, true))
            << std::endl;
  std::cout << v.weight(v.constant(7)) << std::endl;
}

void test_shape() {
  uasat::shape_t s0 = uasat::shape_t();
  uasat::shape_t s1 = uasat::shape_t(2, s0);
  uasat::shape_t s2 = uasat::shape_t(3, s1);
  std::cout << s0 << " " << s0.length() << " " << s0.extent() << std::endl;
  std::cout << s1 << " " << s1.length() << " " << s1.extent() << std::endl;
  std::cout << s2 << " " << s2.length() << " " << s2.extent() << std::endl;
}

int main() {
  // test_binarynum();
  test_shape();
  return 0;
}
