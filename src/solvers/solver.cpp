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

#include <memory>
#include <stdexcept>

#include "minisat.hpp"
#include "uasat/solver.hpp"

namespace uasat {

std::unique_ptr<Solver> Solver::create(const std::string &options) {
  if (options == "minisat")
    return std::unique_ptr<MiniSat>(new MiniSat());

  throw std::invalid_argument("invalid solver");
}

literal_t Solver::land(literal_t a, literal_t b) {
  if (a == -TRUE || b == -TRUE)
    return -TRUE;
  else if (a == TRUE)
    return b;
  else if (b == TRUE)
    return a;
  else if (a == b)
    return a;
  else if (a == -b)
    return -TRUE;

  literal_t c = add_variable(false);
  add_clause(a, -c);
  add_clause(b, -c);
  add_clause(-a, -b, c);
  return c;
}

literal_t Solver::lor(literal_t a, literal_t b) {
  if (a == TRUE || b == TRUE)
    return TRUE;
  else if (a == -TRUE)
    return b;
  else if (b == -TRUE)
    return a;
  else if (a == b)
    return a;
  else if (a == -b)
    return TRUE;

  literal_t c = add_variable(false);
  add_clause(-a, c);
  add_clause(-b, c);
  add_clause(a, b, -c);
  return c;
}

literal_t Solver::ladd(literal_t a, literal_t b) {
  if (a == TRUE)
    return -b;
  else if (b == TRUE)
    return -a;
  else if (a == -TRUE)
    return b;
  else if (b == -TRUE)
    return a;
  else if (a == b)
    return -TRUE;
  else if (a == -b)
    return TRUE;

  literal_t c = add_variable(false);
  add_clause(a, b, -c);
  add_clause(-a, b, c);
  add_clause(a, -b, c);
  add_clause(-a, -b, -c);
  return c;
}

literal_t Solver::lequ(literal_t a, literal_t b) {
  if (a == TRUE)
    return b;
  else if (b == TRUE)
    return a;
  else if (a == -TRUE)
    return -b;
  else if (b == -TRUE)
    return -a;
  else if (a == b)
    return TRUE;
  else if (a == -b)
    return -TRUE;

  literal_t c = add_variable(false);
  add_clause(a, b, c);
  add_clause(-a, b, -c);
  add_clause(a, -b, -c);
  add_clause(-a, -b, c);
  return c;
}

} // namespace uasat
