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

#ifndef __UASAT_MINISAT_HPP__
#define __UASAT_MINISAT_HPP__

#include <memory>
#include <stdexcept>

#include "minisat/core/Solver.h"
#include "uasat/solver.hpp"

namespace uasat {

class MiniSat : public Solver {
protected:
  std::unique_ptr<Minisat::Solver> solver;
  bool solvable;

  static literal_t var2gen(Minisat::Var var) { return var + 1; }

  static Minisat::Lit gen2lit(literal_t lit) {
    return Minisat::toLit(lit > 0 ? (lit << 1) - 2 : ((-lit) << 1) - 1);
  }

public:
  MiniSat();

  virtual literal_t add_literal() override;
  virtual void add_clause(const std::vector<literal_t> &clause) override;
  virtual void add_clause(literal_t lit1) override;
  virtual void add_clause(literal_t lit1, literal_t lit2) override;
  virtual void add_clause(literal_t lit1, literal_t lit2,
                          literal_t lit3) override;

  virtual void clear() override;
  virtual bool solve() override;

  virtual unsigned long get_variable_count() const override;
  virtual unsigned long get_clause_count() const override;
};

} // namespace uasat

#endif // __UASAT_SOLVER_HPP__
