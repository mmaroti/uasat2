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

#include "uasat/solver.hpp"

namespace Minisat {
class Solver;
}

namespace uasat {

class MiniSat : public Solver {
protected:
  std::unique_ptr<Minisat::Solver> solver;
  bool solvable;

public:
  MiniSat();
  virtual ~MiniSat();
  virtual void clear() override;

  virtual literal_t add_variable(bool decision = false) override;
  virtual void add_clause(const std::vector<literal_t> &clause) override;
  virtual void add_clause(literal_t lit1) override;
  virtual void add_clause(literal_t lit1, literal_t lit2) override;
  virtual void add_clause(literal_t lit1, literal_t lit2,
                          literal_t lit3) override;

  virtual unsigned long get_variables() const override;
  virtual unsigned long get_clauses() const override;

  virtual bool solve() override;
  virtual bool get_value(literal_t lit) const override;
};

} // namespace uasat

#endif // __UASAT_SOLVER_HPP__
