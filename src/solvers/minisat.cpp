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
#include "minisat/core/Solver.h"
#include "minisat/simp/SimpSolver.h"
#include <cassert>
#include <stdexcept>

namespace uasat {

inline literal_t var2gen(Minisat::Var var) { return var + 1; }

inline Minisat::Lit gen2lit(literal_t lit) {
  return Minisat::toLit(lit > 0 ? (lit << 1) - 2 : ((-lit) << 1) - 1);
}

MiniSat::MiniSat() { clear(); }

MiniSat::~MiniSat() {}

void MiniSat::clear() {
  solver = std::unique_ptr<Minisat::Solver>(new Minisat::Solver());
  literal_t lit = var2gen(solver->newVar());
  if (lit != TRUE)
    throw new std::logic_error("First literal of MiniSat is not 1");
  solver->addClause(gen2lit(lit));
  solvable = true;
}

literal_t MiniSat::add_variable(bool decision, bool polarity) {
  return var2gen(solver->newVar(polarity, decision));
}

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

unsigned long MiniSat::get_variables() const {
  return std::max(solver->nVars(), 1) - 1;
}

unsigned long MiniSat::get_clauses() const { return solver->nClauses(); }

bool MiniSat::solve() {
  if (solvable)
    solvable = solver->solve();
  return solvable;
}

literal_t MiniSat::get_solution(literal_t lit) const {
  assert(solvable);

  Minisat::lbool value = solver->modelValue(gen2lit(lit));
  return value.isTrue() ? TRUE : value.isFalse() ? FALSE : UNDEF;
}

MiniSatSimp::MiniSatSimp() { clear(); }

MiniSatSimp::~MiniSatSimp() {}

void MiniSatSimp::clear() {
  solver = std::unique_ptr<Minisat::SimpSolver>(new Minisat::SimpSolver());
  literal_t lit = var2gen(solver->newVar());
  if (lit != TRUE)
    throw new std::logic_error("First literal of MiniSat is not 1");
  solver->addClause(gen2lit(lit));
  solvable = true;
  simplified = false;
}

literal_t MiniSatSimp::add_variable(bool decision, bool polarity) {
  Minisat::Var var = solver->newVar(polarity, decision);
  if (decision)
    solver->setFrozen(var, true);
  return var2gen(var);
}

void MiniSatSimp::add_clause(const std::vector<literal_t> &clause) {
  if (solvable) {
    Minisat::vec<Minisat::Lit> vec(clause.size());
    for (size_t i = 0; i < clause.size(); i++)
      vec[i] = gen2lit(clause[i]);

    solvable = solver->addClause(vec);
  }
}

void MiniSatSimp::add_clause(literal_t lit1) {
  if (solvable)
    solvable = solver->addClause(gen2lit(lit1));
}

void MiniSatSimp::add_clause(literal_t lit1, literal_t lit2) {
  if (solvable)
    solvable = solver->addClause(gen2lit(lit1), gen2lit(lit2));
}

void MiniSatSimp::add_clause(literal_t lit1, literal_t lit2, literal_t lit3) {
  if (solvable)
    solvable = solver->addClause(gen2lit(lit1), gen2lit(lit2), gen2lit(lit3));
}

unsigned long MiniSatSimp::get_variables() const {
  return std::max(solver->nVars(), 1) - 1;
}

unsigned long MiniSatSimp::get_clauses() const { return solver->nClauses(); }

bool MiniSatSimp::solve() {
  if (solvable) {
    if (!simplified) {
      solver->eliminate(true);
      solvable = solver->solve(true, true);
    } else
      solvable = solver->solve(false, false);
  }
  return solvable;
}

literal_t MiniSatSimp::get_solution(literal_t lit) const {
  assert(solvable);

  Minisat::lbool value = solver->modelValue(gen2lit(lit));
  return value.isTrue() ? TRUE : value.isFalse() ? FALSE : UNDEF;
}

} // namespace uasat
