//                                                         Vectorization Notes:
//                                                            Day 1, 08/08/2025

/* BASICS:
 *
 *
 *  -> Intel AVX-512 is a vector machine with 512-bits registers.
 *  -> The machine has 32 * 512-bits registers in total.
 *  -> Each of them (each 512-bits register) is referred as ZMM register.
 *  -> The least significant 256 bits of a ZMM is reffered as YMM.
 *  -> The least significant 128 bits of a ZMM is reffered as XMM.
 *
 *                      ZMM
 *    -------------------------------------------
 *    |                   |          |   XMM    |
 *    -------------------------------------------
 *                                YMM
 *
 *  -> In one ZMM (512 bits), there could fit:
 *    8   integers   (64 bits)
 *    16  floats     (32 bits)
 *    64  int8s      (08 bits)
 *
 */

/* BASICS OPERATIONS:
 *
 *  Load & Store: Pushing or retrieving data from memory to the ZMM registers.
 *
 *    -> Unaligned & Aligned Data:
 *      Aligned means address is multiply of data size
 *      in bytes. For SIMD, it means aligned to the register width in bytes.
 *      Aligned data improves the performance since it eliminates reading values
 *      from different cache lines. In modern intel architectures, the size of a
 *      cache line is 64 bytes, same as a size of a ZMM register.
 *      If the data address is not aligned to 64 bytes, the CPU may have to
 *      fetch two different cache lines and then merge them in load buffer.
 *      -- Consider the request hits a page boundary
 *
 *      -> alignas(N) Tells compiler to place this object at an address of
 *      multiple of N.
 *
 */

/* EXAMPLES */

#include <immintrin.h>
#include <iostream>

#define N 16

int main(int argc, char *argv[]) {
  // Defining a __m512 type which is a 8 * 64-bits vector
  // typedef float __m512 __attribute__((__vector_size__(64), __aligned__(64)));
  // Explanation of the type definition is "make this a vector of size 64
  // bytes and align address to 64".
  std::cout << "Hello, AVX-512 machine." << std::endl;

  __m512 zmm0, zmm1;
  alignas(64) float a[N], b[N]; // 16 * 4 bytes * 8 bits per byte

  // Initiliaze the elements of the array
  for (int i = 0; i < N; i++) {
    a[i] = i + argc;
    b[i] = i + argc;
  }

  // Loading arrays to the vector registers
  zmm0 = _mm512_load_ps(a); // 1 cycle
  zmm1 = _mm512_load_ps(b); // 1 cycle

  // 1 zmm -> 64 bytes or pixels
  // 16 pairs of zmm -> 16 * 64 bytes pixels could be added in a single

  // Saving vector addition to another vector
  __m512 result = _mm512_add_ps(zmm0, zmm1); // Add cycle

  // Store back the result of vector addition
  _mm512_store_ps(a, result);

  // Print result to the stdout
  for (int i = 0; i < N; i++) {
    std::cout << i << ": " << a[i] << std::endl;
  }

  return 0;
}
