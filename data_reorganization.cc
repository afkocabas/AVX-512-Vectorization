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
#include <immintrin.h>
#include <iostream>

// DEFINING ARR_SIZE
const int ARR_SIZE = 100000;

struct Point {
  int32_t x, y, z;
  Point() : x(0), y(0), z(0) {};
  Point(int x, int y, int z) : x(x), y(y), z(z) {};
};

// FUNCTION DEFINITIONS
std::chrono::microseconds scalarAdd(std::array<Point, ARR_SIZE> &points);
std::chrono::microseconds vectorAdd(std::array<Point, ARR_SIZE> &points);

int main(int argc, char *argv[]) {
  // CREATING POINT ARRAY AND INITILIAZE THE POINTS
  std::array<Point, ARR_SIZE> points;

  for (int i = 0; i < ARR_SIZE; ++i) {
    Point point(i + argc, i + argc, i + argc);
    points[i] = point;
  }

  auto scalarDuration = scalarAdd(points);

  std::cout << "Scalar add took " << scalarDuration.count() << " microseconds."
            << std::endl;

  return 0;
}

std::chrono::microseconds scalarAdd(std::array<Point, ARR_SIZE> &points) {

  std::array<Point, ARR_SIZE> results;
  Point barrier;

  // ADDITION OF POINTS
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < ARR_SIZE; ++i) {
    auto first = points[i];
    auto second = points[(i + ARR_SIZE / 2) % ARR_SIZE];
    results[i] =
        Point(first.x + second.x, first.y + second.y, first.z + second.z);
    barrier.x = results[i].x;
    barrier.y = results[i].y;
    barrier.z = results[i].z;
  }
  auto end = std::chrono::high_resolution_clock::now();
  //
  asm volatile("" ::"g"(barrier) : "memory");

  return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}
