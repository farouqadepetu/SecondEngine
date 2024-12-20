#pragma once

#include <cmath>
#include <cfloat>

#define PI 3.14159f

inline bool CompareFloats(float a, float b, float maxRelDiff = FLT_EPSILON)
{
    // Calculate the difference.
    float diff = fabs(a - b);
    a = fabs(a);
    b = fabs(b);
    // Find the largest
    float largest = (b > a) ? b : a;

    if (diff <= largest * maxRelDiff)
        return true;
    return false;
}

inline bool CompareDoubles(double a, double b, double maxRelDiff = DBL_EPSILON)
{
    // Calculate the difference.
    double diff = fabs(a - b);
    a = fabs(a);
    b = fabs(b);
    // Find the largest
    double largest = (b > a) ? b : a;

    if (diff <= largest * maxRelDiff)
        return true;
    return false;
}