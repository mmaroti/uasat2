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

#ifndef __UASAT_SOLVER_HPP__
#define __UASAT_SOLVER_HPP__

#include <memory>
#include <vector>

namespace uasat {

typedef int literal_t;
inline literal_t negate(literal_t lit) { return -lit; }

class Solver {
protected:
  literal_t TRUE;

public:
  static std::unique_ptr<Solver> create(const std::string &options = "");
  virtual ~Solver() = default;

  virtual literal_t add_literal() = 0;
  virtual void add_clause(const std::vector<literal_t> &clause) = 0;
  virtual void add_clause(literal_t lit1) { add_clause({lit1}); }
  virtual void add_clause(literal_t lit1, literal_t lit2) {
    add_clause({lit1, lit2});
  }
  virtual void add_clause(literal_t lit1, literal_t lit2, literal_t lit3) {
    add_clause({lit1, lit2, lit3});
  }

  virtual void clear() = 0;
  virtual bool solve() = 0;

  virtual unsigned long get_variable_count() const = 0;
  virtual unsigned long get_clause_count() const = 0;

  literal_t get_true() const { return TRUE; }
  literal_t get_false() const { return -TRUE; }
};

} // namespace uasat

#endif // __UASAT_SOLVER_HPP__