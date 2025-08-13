//                                                         Vectorization Notes:
//                                                            Day 4, 12/08/2025

/* DATA LAYOUTS AND ORGANIZATION: Some data layouts are more preffaable by the
 * programmers --- sometimes for the program logic as well, since it matches how
 * humankind's brain operate. However, these layouts sometimes cannot fully
 * utilize SIMD processors. In these cases, the data should be reorganized to
 * maximize SIMD throughput.
 */

/* AoS vs. SoA:
 *  AoS:
 *  -> Array of Structures: [ point{x, y, z} point{x, y, z} point{x, y, z} ]
 *  -> AoS is in general how we think, right? There are many points and every
 *     point has x-y-z coordinates.
 *  SoA:
 *  -> Structure of Arrays: [ {x1, x2, x3} {y1, y2, y3} {z1, z2, z3} ]
 *  -> This data arragement is more functional for SIMD
 *     machines since each corresponding field of the structruces could be
 *     properly aligned.
 *
 */

/* EXAMPLE */
#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>

// DEFINING ARR_SIZE
const long long ARR_SIZE = 5 * 10000000;

struct Point {
  int32_t x, y, z;
  Point() : x(0), y(0), z(0) {};
  Point(int x, int y, int z) : x(x), y(y), z(z) {};
};

// FUNCTION DEFINITIONS
std::chrono::microseconds scalarAdd();
std::chrono::microseconds vectorAdd();

alignas(64) std::array<Point, ARR_SIZE> results;
alignas(64) std::array<Point, ARR_SIZE> points1;
alignas(64) std::array<Point, ARR_SIZE> points2;

// CREATING POINT ARRAY AND INITILIAZE THE POINTS

int main(int argc, char *argv[]) {
  for (int i = 0; i < ARR_SIZE; i++) {
    Point point(i + argc, i + argc, i + argc);
    points1[i] = point;
    points2[i] = point;
  }

#if USE_AVX512
  auto elapsed = vectorAdd();
  std::cout << "Vector add took " << elapsed.count() << " microseconds\n";
#else
  auto elapsed = scalarAdd();
  std::cout << "Scalar add took " << elapsed.count() << " microseconds\n";
#endif
  return 0;
}
#if !USE_AVX512
std::chrono::microseconds scalarAdd() {
  Point barrier;

  auto start = std::chrono::high_resolution_clock::now();

  // ADDITION OF POINTS
  for (int i = 0; i < ARR_SIZE; ++i) {
    auto first = points1[i];
    auto second = points2[i];
    results[i] =
        Point(first.x + second.x, first.y * second.y, first.z * second.z);
    barrier.x = results[i].x;
    barrier.y = results[i].y;
    barrier.z = results[i].z;
  }
  auto end = std::chrono::high_resolution_clock::now();

  // FORCE COMPILER NOT TO OPTIMIZE THE FOR LOOP
  asm volatile("" ::"g"(barrier) : "memory");

  // RETURN TIME ELAPSED IN MICROSECONDS
  return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}
#else
#include <immintrin.h>

std::chrono::microseconds vectorAdd() {
  __m512i permutation_order = _mm512_set_epi32(
      // x's
      0, 3, 6, 9, 12,
      // y's
      1, 4, 7, 10, 13,
      // z's
      2, 5, 8, 11, 14, 15);

  auto start = std::chrono::high_resolution_clock::now();

  __mmask16 store_mask = 0x7FFF; // bits 0–14 set, skip lane 15
  __mmask16 add_mask = 0xF800;
  for (int i = 0; i < ARR_SIZE / 5; i += 5) {

    // __mmask16 mul_mask = ~add_mask;

    __m512i first = _mm512_load_epi32(&points1[i]);
    __m512i second = _mm512_load_epi32(&points2[i]);

    __m512i first_per = _mm512_permutexvar_epi32(permutation_order, first);
    __m512i second_per = _mm512_permutexvar_epi32(permutation_order, second);

    auto prod = _mm512_mullo_epi32(first_per, second_per);

    __m512i result = _mm512_permutexvar_epi32(
        permutation_order,
        _mm512_mask_add_epi32(prod, add_mask, first_per, second_per));
    _mm512_mask_store_epi32(&results, store_mask, result);
  }

  auto end = std::chrono::high_resolution_clock::now();

  // FORCE COMPILER NOT TO OPTIMIZE THE FOR LOOP

  return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}

#endif // USE_AVX512
