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

#ifndef UASAT_BITVEC_HPP
#define UASAT_BITVEC_HPP

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>

namespace uasat {

struct bitvec_t {
private:
  uint64_t length; // in bits
  uint64_t data[1];

public:
  /**
   * Cannot be publicly created.
   */
  bitvec_t() = delete;

  /**
   * Creates a new bit vector with the given length. The vector must be
   * destroyed when no longer needed.
   */
  static bitvec_t *create(uint64_t length) {
    assert(length <= std::numeric_limits<uint64_t>::max() - 63);

    uint64_t blocks = (length + 63) / 64 + 1;
    bitvec_t *vec = static_cast<bitvec_t *>(std::malloc(8 * blocks));
    vec->length = length;
    return vec;
  }

  /**
   * Destroyes the given bit vector. The same vector cannot be destroyed twice.
   */
  static void destroy(bitvec_t *vec) {
    assert(vec->length <= std::numeric_limits<uint64_t>::max() - 63);
#ifndef NDEBUG
    vec->length = std::numeric_limits<uint64_t>::max();
#endif

    std::free(vec);
  }

  /**
   * Copies the data from source to destination whose lengths must match.
   */
  static void copy(bitvec_t *dst, bitvec_t *src) {
    assert(dst->length == src->length);
    std::memcpy(dst->data, src->data, (dst->length + 7) / 8);
  }

  static void negate(bitvec_t *dst, bitvec_t *src) {
    assert(dst->length == src->length);

    uint64_t blocks = (dst->length + 63) / 64;
    for (uint64_t i = 0; i < blocks; i++)
      dst->data[i] = ~(src->data[i]);
  }
};

} // namespace uasat

#endif // UASAT_BITVEC_HPP
