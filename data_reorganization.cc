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
#include <immintrin.h>

const int ARR_SIZE = 1000;

struct Point {
  int32_t x, y, z;
  Point() : x(0), y(0), z(0) {};
  Point(int x, int y, int z) : x(x), y(y), z(z) {};
};

int main(int argc, char *argv[]) {
  // Create point array and initiaze the points
  std::array<Point, ARR_SIZE> points;
  for (int i = 0; i < ARR_SIZE; ++i) {
    Point point(i + argc, i + argc, i + argc);
    points[i] = point;
  }
  return 0;
}
