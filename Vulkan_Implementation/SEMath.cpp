#define USE_SIMD 1

#if USE_SIMD
#include "vectors_intrinsics.cpp"
#include "matrices_intrinsics.cpp"
#include "quaternion_intrinsics.cpp"

#else
#include "vectors.cpp"
#include "matrices.cpp"
#include "quaternion.cpp"

#endif