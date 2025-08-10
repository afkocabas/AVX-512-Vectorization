//                                                         Vectorization Notes:
//                                                            Day 2, 09/08/2025

/*
 * TAIL PROCESSING: When the number of elements is not the multiple of size of
 * a vector register, some elements could remain unoperated. These elements are
 * called "tail elements".
 *
 *
 *   -> Example: Adding two array of 10-int64
 *   %ZMM vector register size is 64 bytes.
 *   Two of int[8] a, b -> 2 *(10 elements * 8 bytes) -> 2 * 80 bytes
 *   After loading from the base addresses (&a and &b), two elements per array
 *   remain unloaded or unoperated.
 *
 *
 *    -> To keep everything consistent and vectorized, the vector register could
 *    still be utilized -- no need to fall back to scalar. However, tail
 *    processing does not require entire register 512-bits to be used. Instead,
 *    some portion of the register could be loaded. The mechanism allowing it is
 *    called "masking".
 *
 *
 * MASKING: Activating/Deactivating a vector machine's register lanes during a
 * vector instruction.
 *
 *                                     %ZMM
 *        --------------------------------------------------------------
 *        |     |      |      |      |       |       |        |        |
 *        --------------------------------------------------------------
 *          x      x      x      x       x       x      int       int
 *
 */

/* EXAMPLE */

#include <immintrin.h>
#include <iostream>

// Arrays' size
#define N 18

int main(int argc, char *argv[]) {

  // Declaring arrays as aligned
  alignas(64) int64_t a[N], b[N], c[N];

  // Initiliaze the elements of the array
  for (int i = 0; i < N; i++) {
    a[i] = i + argc;
    b[i] = i + argc;
  }

  int i = 0;
  size_t count = 1;

  /* The next iteration is where vector chunks are operated. While operating,
   * the loop also checks whether there are still enough number of elements left
   * to fill the entire %ZMM register. At the end i points leftovers if any.
   */
  for (; i + 7 < N; i += 8) {
    std::cout << "Vector Iteration: " << count++ << " | i = " << i << std::endl;
    auto vA = _mm512_load_epi64(a + i);
    auto vB = _mm512_load_epi64(b + i);
    _mm512_store_epi64(c + i, _mm512_add_epi64(vA, vB));
  }

  /* If leftover != 0, it means that there are that many elements
   * unprocessed. For example, if there are 18 elements in the arrays, At the
   * end, i will be 16. This means that elements starting from index i included
   * up to N - 1, where N is the number of elements, are leftovers (i.e
   * array[16] & array[17] are left unoperated.)
   */

  auto leftover = N - i;
  if (leftover != 0) {
    std::cout << "There are " << leftover << " leftovers.\n";

    // Creating the mask for the next vector load
    __mmask8 mask = (1u << leftover) - 1u;

    /* i.e leftover is three:
     * 1u                 0000 0000 0000 0000 0000 0000 0000 0001
     * Shifted value      0000 0000 0000 0000 0000 0000 0000 1000
     * Shifted value - 1: 0000 0000 0000 0000 0000 0000 0000 0111
     * After type conversion to (__mmask8) -> 0000 0111 => 8 lanes three of
     * which are active. Three lanes activated 5 lanes deactivated.
     */

    /* Since the only leftover elements should
     * be operated, the arrays' base pointers should be shifted by "i" Where i
     * is the number of total elements already operated.
     */
    auto vA = _mm512_maskz_load_epi64(mask, a + i);
    auto vB = _mm512_maskz_load_epi64(mask, b + i);

    _mm512_mask_store_epi64(c + i, mask, _mm512_add_epi64(vA, vB));
  }

  // Printing the result
  for (int i = 0; i < N; ++i) {
    std::cout << i << " :" << c[i] << std::endl;
  }

  return 0;
}
