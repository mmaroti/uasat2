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

#include "minisat.hpp"

namespace uasat {

MiniSat::MiniSat() { clear(); }

void MiniSat::clear() {
  solver = std::unique_ptr<Minisat::Solver>(new Minisat::Solver());
  TRUE = var2gen(solver->newVar());
  solver->addClause(gen2lit(TRUE));
  solvable = true;
}

literal_t MiniSat::add_literal() { return var2gen(solver->newVar()); }

void MiniSat::add_clause(const std::vector<literal_t> &clause) {
  if (solvable) {
    Minisat::vec<Minisat::Lit> vec(clause.size());
    for (size_t i = 0; i < clause.size(); i++)
      vec[i] = gen2lit(clause[i]);

    solvable = solver->addClause(vec);
  }
}

void MiniSat::add_clause(literal_t lit1) {
  if (solvable)
    solvable = solver->addClause(gen2lit(lit1));
}

void MiniSat::add_clause(literal_t lit1, literal_t lit2) {
  if (solvable)
    solvable = solver->addClause(gen2lit(lit1), gen2lit(lit2));
}

void MiniSat::add_clause(literal_t lit1, literal_t lit2, literal_t lit3) {
  if (solvable)
    solvable = solver->addClause(gen2lit(lit1), gen2lit(lit2), gen2lit(lit3));
}

bool MiniSat::solve() {
  if (solvable)
    solvable = solver->solve();
  return solvable;
}

unsigned long MiniSat::get_variable_count() const {
  return std::max(solver->nVars(), 1) - 1;
}

unsigned long MiniSat::get_clause_count() const {
  return solver->nClauses();
}

} // namespace uasat