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

#include <algorithm>

#include "solvers/minisat.hpp"
#include "uasat/solver.hpp"

namespace uasat {

literal_t Logic::fold_all(const std::vector<literal_t> &literals) {
  literal_t res = TRUE;
  for (literal_t lit : literals)
    res = logic_and(res, lit);
  return res;
}

literal_t Logic::fold_any(const std::vector<literal_t> &literals) {
  literal_t res = FALSE;
  for (literal_t lit : literals)
    res = logic_or(res, lit);
  return res;
}

literal_t Logic::fold_sum(const std::vector<literal_t> &literals) {
  literal_t res = TRUE;
  for (literal_t lit : literals)
    res = logic_add(res, lit);
  return res;
}

class Boolean : public Logic {
public:
  virtual literal_t logic_and(literal_t lit1, literal_t lit2) override {
    return lit1 <= lit2 ? lit1 : lit2;
  }

  virtual literal_t logic_add(literal_t lit1, literal_t lit2) override {
    literal_t s = lit1 ^ lit2;                   // sign(lit1*b)
    lit1 = lit1 < 0 ? -lit1 : lit1;              // abs(lit1)
    lit2 = lit2 < 0 ? -lit2 : lit2;              // abs(lit2)
    literal_t lit3 = lit1 <= lit2 ? lit1 : lit2; // min(lit1,lit2)
    return s < 0 ? -lit3 : lit3;
  }
};

const std::shared_ptr<Logic> BOOLEAN = std::make_shared<Boolean>();

std::shared_ptr<Solver> Solver::create(const std::string &options) {
  if (options == "minisat")
    return std::make_shared<MiniSat>();
  else if (options == "minisatsimp")
    return std::make_shared<MiniSatSimp>();

  throw std::invalid_argument("invalid solver");
}

literal_t Solver::logic_and(literal_t lit1, literal_t lit2) {
  if (lit1 == FALSE || lit2 == FALSE)
    return FALSE;
  else if (lit1 == TRUE)
    return lit2;
  else if (lit2 == TRUE)
    return lit1;
  else if (lit1 == lit2)
    return lit1;
  else if (lit1 == logic_not(lit2))
    return FALSE;

  literal_t lit3 = add_variable(false, false);
  add_clause(lit1, logic_not(lit3));
  add_clause(lit2, logic_not(lit3));
  add_clause(logic_not(lit1), logic_not(lit2), lit3);
  return lit3;
}

literal_t Solver::logic_add(literal_t lit1, literal_t lit2) {
  if (lit1 == FALSE)
    return lit2;
  else if (lit2 == FALSE)
    return lit1;
  else if (lit1 == TRUE)
    return logic_not(lit2);
  else if (lit2 == TRUE)
    return logic_not(lit1);
  else if (lit1 == lit2)
    return FALSE;
  else if (lit1 == logic_not(lit2))
    return TRUE;

  literal_t lit3 = add_variable(false, false);
  add_clause(lit1, lit2, logic_not(lit3));
  add_clause(logic_not(lit1), lit2, lit3);
  add_clause(lit1, logic_not(lit2), lit3);
  add_clause(logic_not(lit1), logic_not(lit2), logic_not(lit3));
  return lit3;
}

literal_t Solver::fold_all(const std::vector<literal_t> &literals) {
  std::vector<literal_t> clause;
  clause.reserve(literals.size() + 1);

  for (const literal_t &lit : literals) {
    if (lit == FALSE ||
        std::find(clause.begin(), clause.end(), logic_not(lit)) != clause.end())
      return FALSE;
    else if (lit == TRUE ||
             std::find(clause.begin(), clause.end(), lit) != clause.end())
      continue;
    else
      clause.push_back(logic_not(lit));
  }

  if (clause.size() == 0)
    return TRUE;
  else if (clause.size() == 1)
    return clause.front();

  literal_t lit2 = add_variable(false, false);
  for (const literal_t &lit : clause)
    add_clause(logic_not(lit), logic_not(lit2));

  clause.push_back(lit2);
  add_clause(clause);

  return lit2;
}

literal_t Solver::fold_any(const std::vector<literal_t> &literals) {
  std::vector<literal_t> clause;
  clause.reserve(literals.size() + 1);

  for (const literal_t &lit : literals) {
    if (lit == TRUE ||
        std::find(clause.begin(), clause.end(), logic_not(lit)) != clause.end())
      return TRUE;
    else if (lit == FALSE ||
             std::find(clause.begin(), clause.end(), lit) != clause.end())
      continue;
    else
      clause.push_back(lit);
  }

  if (clause.size() == 0)
    return FALSE;
  else if (clause.size() == 1)
    return clause.front();

  literal_t lit2 = add_variable(false, false);
  for (const literal_t &lit : clause)
    add_clause(logic_not(lit), lit2);

  clause.push_back(logic_not(lit2));
  add_clause(clause);

  return lit2;
}

} // namespace uasat
