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
