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

#include <cassert>
#include <stdexcept>

#include "uasat/tensor.hpp"

typedef uasat::Tensor Tensor;
typedef uasat::literal_t literal_t;

struct InitOp {
  int size;

  enum initop_t {
    MM_SET1_EPI8 = 0,
    MM_SET1_EPI2, // nonstandard
    CONSTOP_NUM
  };

  Tensor eval(initop_t type, Tensor init) const {
    assert(init.get_shape().size() == 1);
    int size2 = init.get_shape()[0];

    switch (type) {
    case MM_SET1_EPI8:
      assert(size % 8 == 0 && size2 == 8);
      return init.polymer({size2, size / size2}, {0}).reshape(2, {size});

    case MM_SET1_EPI2:
      assert(size % 2 == 0 && size2 == 2);
      return init.polymer({size2, size / size2}, {0}).reshape(2, {size});

    default:
      throw std::logic_error("unknown init operation");
    }
  };
};

struct BinOp {
  int size;

  enum binop_t {
    MM_AND_XXX = 0,
    MM_ANDNOT_XXX,
    MM_OR_XXX,
    MM_XOR_XXX,
    MM_ADD_EPI8,
    MM_SUB_EPI8,
    BINOP_NUM,
  };

  Tensor eval(binop_t type, Tensor reg1, Tensor reg2) const {
    assert(reg1.get_shape().size() == 1 && reg1.get_shape()[0] == size);
    assert(reg1.get_shape().size() == 1 && reg1.get_shape()[0] == size);
    std::shared_ptr<uasat::Logic> logic3 =
        uasat::Logic::join(reg1.get_logic(), reg2.get_logic());

    switch (type) {
    case MM_AND_XXX:
      return reg1.logic_and(reg2);

    case MM_ANDNOT_XXX:
      return reg1.logic_not().logic_and(reg2);

    case MM_OR_XXX:
      return reg1.logic_or(reg2);

    case MM_XOR_XXX:
      return reg1.logic_add(reg2);

    case MM_ADD_EPI8: {
      assert(size % 8 == 0);
      Tensor reg3 = Tensor::constant({size}, false);
      for (int i = 0; i < size; i += 8) {
        literal_t carry = logic3->FALSE;
        for (int j = i; j < i + 8; j++) {
          literal_t a = reg1.__very_slow_get_value({j});
          literal_t b = reg2.__very_slow_get_value({j});
          literal_t c = logic3->full_adder(a, b, carry);
          reg3.__very_slow_set_value({j}, c);
        }
      }
      return reg3;
    }

    case MM_SUB_EPI8: {
      assert(size % 8 == 0);
      reg2 = reg2.logic_not();
      Tensor reg3 = Tensor::constant({size}, false);
      for (int i = 0; i < size; i += 8) {
        literal_t carry = logic3->TRUE;
        for (int j = i; j < i + 8; j++) {
          literal_t a = reg1.__very_slow_get_value({j});
          literal_t b = reg2.__very_slow_get_value({j});
          literal_t c = logic3->full_adder(a, b, carry);
          reg3.__very_slow_set_value({j}, c);
        }
      }
      return reg3;
    }

    default:
      throw std::logic_error("unknown binary operation");
    }
  };
};
