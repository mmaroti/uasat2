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
#include <cmath>
#include <iostream>

static constexpr int C = 5;
static constexpr int N = 100;
static constexpr float T = 0.03f;
static constexpr float E = 1.42f * T;

static int lit[N][N][C];
static std::vector<std::array<int, 2>> circle;

int xmain() {
  std::shared_ptr<uasat::Solver> solver = uasat::Solver::create();

  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      for (int c = 0; c < C; c++)
        lit[i][j][c] = solver->add_variable();

  std::vector<int> cla(C);
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++) {
      for (int c = 0; c < C; c++)
        cla[c] = lit[i][j][c];
      solver->add_clause(cla);
    }

  const int K = std::ceil((1.0f + E) / T);
  for (int i = 0; i <= K; i++)
    for (int j = -K; j <= K; j++) {
      if (i == 0 && j < 0)
        continue;
      float a = i * T;
      float b = j * T;
      float c = std::abs(std::sqrt(a * a + b * b) - 1.0f);
      if (c < E)
        circle.push_back({i, j});
    }

  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      for (std::array<int, 2> offset : circle) {
        int i2 = i + offset[0];
        int j2 = j + offset[1];
        if (0 <= i2 && i2 < N && 0 <= j2 && j2 < N)
          for (int c = 0; c < C; c++)
            solver->add_clause(solver->logic_not(lit[i][j][c]),
                               solver->logic_not(lit[i2][j2][c]));
      }

  bool b = solver->solve();
  std::cout << b << std::endl;

  return 0;
}
