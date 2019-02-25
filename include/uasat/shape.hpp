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

#ifndef UASAT_SHAPE_HPP
#define UASAT_SHAPE_HPP

#include <cassert>
#include <ostream>
#include <vector>

namespace uasat {

class shape_t {
private:
  struct node_t {
    node_t(unsigned int dim, node_t *next)
        : next(next), ref(1), dim(dim),
          ext(next != NULL ? next->ext * dim : dim) {}

    node_t *next;
    unsigned int ref;
    unsigned int dim;
    unsigned long ext;
  };

  node_t *node;

  shape_t(node_t *node) : node(node) {
    if (node != NULL)
      node->ref += 1;
  }

public:
  shape_t(const shape_t &shape) : node(shape.node) {
    if (node != NULL)
      node->ref += 1;
  }

  shape_t(shape_t &&shape) : node(shape.node) { shape.node = NULL; };

  shape_t() : node(NULL) {}

  shape_t(unsigned int dim, const shape_t &next) {
    if (next.node != NULL)
      next.node->ref += 1;
    node = new node_t(dim, next.node);
  }

  shape_t(const std::vector<int> &shape);

  ~shape_t();

  shape_t &operator=(const shape_t &shape) = delete;

  bool operator==(const shape_t &other) const;

  operator std::vector<int>() const;

  bool is_empty() const { return node == NULL; }

  bool prefix_of(const shape_t &other) const;

  unsigned int head() const {
    assert(node != NULL);
    return node->dim;
  }

  shape_t tail() const {
    assert(node != NULL);
    return shape_t(node->next);
  }

  shape_t tail(unsigned int pos) const;

  unsigned int length() const;

  unsigned long extent() const { return node != NULL ? node->ext : 1; }

  friend std::ostream &operator<<(std::ostream &out, const shape_t &shape);
};

} // namespace uasat

#endif // UASAT_BITVEC_HPP
