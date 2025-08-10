//                                                         Vectorization Notes:
//                                                            Day 3, 10/08/2025

/* GATHERING & SCATTERING: In basics.cc and masks.cc, we dealt with contigious
 * memory access with vector machines. That is, the memory region in the
 * interest is sequential. However, this is not always the case.
 * Sometimes, the data should be loaded/stored from/to non-contigious memory
 * regions.
 *
 *
 * REASONS WHY USING VECTORS TO OPERATE ON NON-CONTIGIOUS MEMORY REGIONS:
 * One can ask the question: Why would vectors be used although the number of
 * memory access are almost the same as it would be in scalar register. Even if
 * it is partially correct that the number of cache lines accessed for the
 * non-contigious memory region results in no improvement in load stage, there
 * is still  improvement in execution cycles in operation stage. For
 * example, after loading the values, the vector multiplication could be
 * conducted in a single operation cycle.
 *
 */

/* EXAMPLE */

#include <immintrin.h>
#include <iostream>

// Number of elements
#define N 128

int main(int argc, char *argv[]) {

  alignas(64) int data[N];

  // Initiliaze the array
  for (int i = 0; i < N; ++i)
    data[i] = i + 1;

  // Elements' indices to be gathered
  __m512i indices = _mm512_set_epi32(1, 5, 10, 7, 20, 40, 35, 53, 73, 2, 6, 11,
                                     3, 18, 31, 69);

  /*
   * Parameters passed:
   *  -> Indices of elements to be gathered
   *  -> Array pointer
   *  -> Size of each element in the array
   */
  auto valuesGathered = _mm512_i32gather_epi32(indices, data, sizeof(int));

  // Defining 16 * 32-bit values as immediate values
  __m512i immediates = _mm512_set1_epi32(100);

  // Vector Addition valuesGathered and Immediates
  auto results = _mm512_add_epi32(valuesGathered, immediates);

  // Scattering the results of the addition operation to the non-contigious
  // memory.
  _mm512_i32scatter_epi32(data, indices, results, 4);

  for (int i = 0; i < N; ++i)
    std::cout << data[i] << std::endl;

  return 0;
}
