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

#include "uasat/solver.hpp"
#include "solvers/minisat.hpp"
#include <algorithm>
#include <cassert>

namespace uasat {

literal_t Logic::logic_add(literal_t lit1, literal_t lit2) {
  return logic_or(logic_and(lit1, logic_not(lit2)),
                  logic_and(logic_not(lit1), lit2));
}

literal_t Logic::logic_maj(literal_t lit1, literal_t lit2, literal_t lit3) {
  return logic_or(logic_or(logic_and(lit1, lit2), logic_and(lit1, lit3)),
                  logic_and(lit2, lit3));
}

literal_t Logic::logic_iff(literal_t lit1, literal_t lit2, literal_t lit3) {
  return logic_or(logic_and(lit1, lit2), logic_and(logic_not(lit1), lit3));
}

std::shared_ptr<Logic> Logic::join(const std::shared_ptr<Logic> &logic1,
                                   const std::shared_ptr<Logic> &logic2) {
  if (logic1 != logic2 && logic1 != BOOLEAN && logic2 != BOOLEAN)
    throw std::invalid_argument("non-matching logics");
  return logic1 != BOOLEAN ? logic1 : logic2;
}

class Boolean : public Logic {
public:
  virtual literal_t logic_and(literal_t lit1, literal_t lit2) override {
    assert(FALSE <= lit1 && lit1 <= TRUE && FALSE <= lit2 && lit2 <= TRUE);
    return lit1 <= lit2 ? lit1 : lit2;
  }

  virtual literal_t logic_add(literal_t lit1, literal_t lit2) override {
    assert(FALSE <= lit1 && lit1 <= TRUE && FALSE <= lit2 && lit2 <= TRUE);
    literal_t sign = lit1 ^ lit2;
    literal_t abs1 = lit1 < 0 ? -lit1 : lit1;
    literal_t abs2 = lit2 < 0 ? -lit2 : lit2;
    literal_t abs3 = abs1 <= abs2 ? abs1 : abs2;
    literal_t lit3 = sign < 0 ? abs3 : -abs3;
    return lit3;
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

literal_t Solver::logic_maj(literal_t lit1, literal_t lit2, literal_t lit3) {
  if (lit1 == FALSE)
    return logic_and(lit2, lit3);
  else if (lit1 == TRUE)
    return logic_or(lit2, lit3);
  else if (lit2 == FALSE)
    return logic_and(lit1, lit3);
  else if (lit2 == TRUE)
    return logic_or(lit1, lit3);
  else if (lit3 == FALSE)
    return logic_and(lit1, lit2);
  else if (lit3 == TRUE)
    return logic_or(lit1, lit2);
  else if (lit1 == lit2)
    return lit1;
  else if (lit1 == logic_not(lit2))
    return lit3;
  else if (lit1 == lit3)
    return lit1;
  else if (lit1 == logic_not(lit3))
    return lit2;
  else if (lit2 == lit3)
    return lit2;
  else if (lit2 == logic_not(lit3))
    return lit1;

  literal_t lit4 = add_variable(false, false);
  add_clause(lit1, lit2, logic_not(lit4));
  add_clause(lit1, lit3, logic_not(lit4));
  add_clause(lit2, lit3, logic_not(lit4));
  add_clause(logic_not(lit1), logic_not(lit2), lit4);
  add_clause(logic_not(lit1), logic_not(lit3), lit4);
  add_clause(logic_not(lit2), logic_not(lit3), lit4);
  return lit4;
}

} // namespace uasat
