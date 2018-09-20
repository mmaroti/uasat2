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
  std::shared_ptr<uasat::Solver> solver = uasat::Solver::create();

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
  std::shared_ptr<uasat::Solver> solver = uasat::Solver::create();

  // create binary relation
  uasat::Tensor relation = uasat::Tensor::variable(solver, {size, size});

  uasat::literal_t reflexive = relation.permute({size}, {0, 0}).fold_all();

  uasat::literal_t symmetric =
      relation.logic_leq(relation.permute({size, size}, {1, 0})).fold_all();

  uasat::literal_t transitive =
      relation.permute({size, size, size}, {1, 0})
          .logic_and(relation.permute({size, size, size}, {0, 2}))
          .fold_any({true, false, false})
          .fold_all();

  uasat::literal_t equivalence =
      solver->fold_all({reflexive, symmetric, transitive});

  int count = 0;
  /*
    while (solver->solve()) {
      count += 1;
      relation->get_solution();
    }
  */

  return count;
}

int main() {
  std::cout << "Calculating the 8th Bell number (4140 solutions): ";

  auto start = std::chrono::steady_clock::now();
  int result = validate1(8);
  int msecs = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::steady_clock::now() - start)
                  .count();

  if (result != 4140)
    std::cout << "incorrect answer of " << result << std::endl;
  else
    std::cout << msecs << " milliseconds" << std::endl;

  return 0;
}
