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

#ifndef __UASAT_BOOLALG_HPP__
#define __UASAT_BOOLALG_HPP__

#include <memory>
#include <string>
#include <vector>

namespace uasat {

typedef int literal_t;

class BoolAlg {
public:
  const literal_t TRUE;
  const literal_t FALSE;

public:
  BoolAlg(literal_t TRUE) : TRUE(TRUE), FALSE(-TRUE) {}
  virtual ~BoolAlg() = default;

  static literal_t lnot(literal_t a) { return -a; }
  virtual literal_t land(literal_t a, literal_t b) { return -lor(-a, -b); }
  virtual literal_t lor(literal_t a, literal_t b) { return -land(-a, -b); }
  virtual literal_t ladd(literal_t a, literal_t b) { return -lxor(a, -b); }
  virtual literal_t lxor(literal_t a, literal_t b) { return -ladd(a, -b); }
};

extern const BoolAlg BINARY;

class Solver : public BoolAlg {
protected:
  Solver(literal_t TRUE) : BoolAlg(TRUE) {}

public:
  static std::shared_ptr<Solver> create(const std::string &options = "minisat");
  virtual ~Solver() = default;
  virtual void clear() = 0;

  virtual literal_t add_variable(bool decision = true) = 0;
  virtual void add_clause(const std::vector<literal_t> &clause) = 0;
  virtual void add_clause(literal_t lit1) = 0;
  virtual void add_clause(literal_t lit1, literal_t lit2) = 0;
  virtual void add_clause(literal_t lit1, literal_t lit2, literal_t lit3) = 0;

  virtual unsigned long get_variables() const = 0;
  virtual unsigned long get_clauses() const = 0;

  virtual bool solve() = 0;
  virtual bool get_value(literal_t) const = 0;

  virtual literal_t land(literal_t a, literal_t b) override;
  virtual literal_t ladd(literal_t a, literal_t b) override;
};

} // namespace uasat

#endif // __UASAT_BOOLALG_HPP__
