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

#ifndef UASAT_SOLVER_HPP
#define UASAT_SOLVER_HPP

#include <memory>
#include <string>
#include <vector>

namespace uasat {

typedef int literal_t;

class Logic {
public:
  static const literal_t TRUE = 1;
  static const literal_t UNDEF = 0;
  static const literal_t FALSE = -1;

public:
  virtual ~Logic() = default;

  literal_t logic_not(literal_t lit) { return -lit; }
  virtual literal_t logic_and(literal_t lit1, literal_t lit2) = 0;
  virtual literal_t logic_add(literal_t lit1, literal_t lit2) = 0;

  literal_t logic_or(literal_t lit1, literal_t lit2) {
    return logic_not(logic_and(logic_not(lit1), logic_not(lit2)));
  }

  literal_t logic_leq(literal_t lit1, literal_t lit2) {
    return logic_not(logic_and(lit1, logic_not(lit2)));
  }

  literal_t logic_equ(literal_t lit1, literal_t lit2) {
    return logic_add(lit1, logic_not(lit2));
  }

  virtual literal_t fold_all(const std::vector<literal_t> &literals);
  virtual literal_t fold_any(const std::vector<literal_t> &literals);
  virtual literal_t fold_sum(const std::vector<literal_t> &literals);
};

extern const std::shared_ptr<Logic> BOOLEAN;

class Tensor;

class Solver : public Logic {
public:
  static std::shared_ptr<Solver> create(const std::string &options = "minisat");
  virtual ~Solver() = default;
  virtual void clear() = 0;

  virtual literal_t add_variable(bool decision = true,
                                 bool polarity = false) = 0;
  virtual void add_clause(const std::vector<literal_t> &clause) = 0;
  virtual void add_clause(literal_t lit1) = 0;
  virtual void add_clause(literal_t lit1, literal_t lit2) = 0;
  virtual void add_clause(literal_t lit1, literal_t lit2, literal_t lit3) = 0;

  virtual unsigned long get_variables() const = 0;
  virtual unsigned long get_clauses() const = 0;

  virtual bool solve() = 0;
  virtual literal_t get_solution(literal_t lit) const = 0;

  literal_t logic_and(literal_t lit1, literal_t lit2) override;
  literal_t logic_add(literal_t lit1, literal_t lit2) override;

  literal_t fold_all(const std::vector<literal_t> &literals) override;
  literal_t fold_any(const std::vector<literal_t> &literals) override;
};

} // namespace uasat

#endif // UASAT_SOLVER_HPP
