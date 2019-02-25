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

#include "uasat/shape.hpp"

namespace uasat {

shape_t::shape_t(const std::vector<int> &shape) {
  node_t *n = NULL;
  std::size_t i = shape.size();
  while (i > 0) {
    int s = shape[--i];
    assert(s >= 0);
    n = new node_t(s, n);
  }
  node = n;
}

shape_t::~shape_t() {
  node_t *n = node;
  while (n != NULL && --(n->ref) == 0) {
    node_t *n2 = n->next;
    delete n;
    n = n2;
  }
}

bool shape_t::operator==(const shape_t &other) const {
  node_t *n1 = node;
  node_t *n2 = other.node;
  for (;;) {
    if (n1 == NULL)
      return n2 == NULL;
    else if (n2 == NULL || n1->dim != n2->dim)
      return false;

    n1 = n1->next;
    n2 = n2->next;
  }
}

shape_t::operator std::vector<int>() const {
  std::vector<int> shape;

  node_t *n = node;
  while (n != NULL) {
    shape.push_back(n->dim);
    n = n->next;
  }

  return shape;
}

bool shape_t::prefix_of(const shape_t &other) const {
  node_t *n1 = node;
  node_t *n2 = other.node;
  for (;;) {
    if (n1 == NULL)
      return true;
    else if (n2 == NULL || n1->dim != n2->dim)
      return false;

    n1 = n1->next;
    n2 = n2->next;
  }
}

shape_t shape_t::tail(unsigned int pos) const {
  node_t *n = node;
  while (pos > 0) {
    assert(n != NULL);
    n = n->next;
  }
  return shape_t(n);
}

unsigned int shape_t::length() const {
  unsigned int r = 0;
  node_t *n = node;
  while (n != NULL) {
    r += 1;
    n = n->next;
  }
  return r;
}

std::ostream &operator<<(std::ostream &out, const shape_t &shape) {
  out << '(';
  shape_t::node_t *n = shape.node;
  if (n != NULL) {
    for (;;) {
      out << n->dim;
      n = n->next;
      if (n == NULL)
        break;
      out << ',';
    }
  }
  out << ')';
  return out;
}

} // namespace uasat
