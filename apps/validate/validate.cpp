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

#include <chrono>
#include <iostream>

#include "uasat/solver.hpp"
#include "uasat/tensor.hpp"

int validate1(int size) {
  std::shared_ptr<uasat::Solver> solver = uasat::Solver::create("minisat");

  // create binary relation
  std::vector<uasat::literal_t> table(size * size);
  for (size_t i = 0; i < table.size(); i++)
    table[i] = solver->add_variable();

  // reflexive
  for (int i = 0; i < size; i++)
    solver->add_clause(table[i * (size + 1)]);

  // symmetric
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      solver->add_clause(table[i * size + j],
                         solver->logic_not(table[j * size + i]));

  // transitive
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      for (int k = 0; k < size; k++)
        solver->add_clause(solver->logic_not(table[i * size + j]),
                           solver->logic_not(table[j * size + k]),
                           table[i * size + k]);

  std::vector<uasat::literal_t> clause(size * size);
  int count = 0;
  while (solver->solve()) {
    count += 1;
    for (size_t i = 0; i < table.size(); i++) {
      uasat::literal_t b = solver->get_solution(table[i]);
      clause[i] = b == solver->TRUE ? solver->logic_not(table[i]) : table[i];
    }
    solver->add_clause(clause);
  }

  return count;
}

int validate2(int size) {
  std::shared_ptr<uasat::Solver> solver = uasat::Solver::create("minisatsimp");

  uasat::Tensor relation = uasat::Tensor::variable(solver, {size, size});

  uasat::Tensor reflexive = relation.polymer({size}, {0, 0}).fold_all();

  uasat::Tensor symmetric =
      relation.logic_leq(relation.polymer({size, size}, {1, 0})).fold_all();

  /*
    uasat::Tensor transitive;
    if (false) {
      transitive = relation.polymer({size, size, size}, {0, 1})
                       .logic_and(relation.polymer({size, size, size}, {1, 2}))
                       .fold_any({false, true, false})
                       .logic_leq(relation)
                       .fold_all();
    } else {
      transitive = relation.polymer({size, size, size}, {0, 1})
                       .logic_and(relation.polymer({size, size, size}, {1, 2}))
                       .logic_leq(relation.polymer({size, size, size}, {0, 2}))
                       .fold_all();
    }
  */

  uasat::Tensor transitive =
      relation.polymer({size, size, size}, {0, 1})
          .logic_and(relation.polymer({size, size, size}, {1, 2}))
          .fold_any({false, true, false})
          .logic_leq(relation)
          .fold_all();

  uasat::Tensor equivalence =
      reflexive.logic_and(symmetric).logic_and(transitive);
  solver->add_clause(equivalence.get_scalar());

  int count = 0;
  while (solver->solve()) {
    std::vector<uasat::literal_t> clause;
    relation.logic_add(relation.get_solution(solver)).get_clause(clause);
    solver->add_clause(clause);
    count += 1;
  }

  return count;
}

int main() {
  std::cout << "Calculating the 8th Bell number (4140 solutions)" << std::endl;

  auto start = std::chrono::steady_clock::now();
  int result = validate2(8);
  int msecs = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::steady_clock::now() - start)
                  .count();

  if (result != 4140)
    std::cout << "Incorrect answer of " << result << std::endl;
  else
    std::cout << "Finished in " << msecs << " milliseconds" << std::endl;

  return 0;
}
