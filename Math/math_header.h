#pragma once

#define USE_SIMD 1

#if USE_SIMD
#include "vectors_intrinsics.h"
#include "matrices_intrinsics.h"
#include "quaternion_intrinsics.h"

#else
#include "vectors.h"
#include "matrices.h"
#include "quaternion.h"

#endif