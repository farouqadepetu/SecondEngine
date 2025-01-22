#pragma once

#define USE_SIMD 1

#if USE_SIMD
#include "SEMath_Intrinsics.h"

#else
#include "SEMath.h"
#endif