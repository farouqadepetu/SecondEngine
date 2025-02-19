#pragma once

#define USE_SIMD 1

#if USE_SIMD
#include "SEMath_Intrinsics.h"

#else
#include "SEMath.h"
#endif

#define PI 3.14159f
#define PI2 6.28319f