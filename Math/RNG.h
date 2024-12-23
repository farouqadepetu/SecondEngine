#pragma once
#include <cstdint>
#include <cstdlib>

//Returns a random uint between [0, 2^32)
inline uint32_t PCG_Hash(uint32_t input)
{
    uint32_t state = input * 747796405u + 2891336453u;
    uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

//Returns a random uint between [0, 2^32)
inline uint32_t RandomUInt(uint32_t& seed)
{
  seed = PCG_Hash(seed);
  return seed;
}

//Returns a random uint between [min, max]
inline uint32_t RandomUInt(uint32_t& seed, uint32_t min, uint32_t max)
{
    seed = PCG_Hash(seed);
    return min + seed % (max - min + 1);
}

//Returns a random int between [-2^31, 2^31)
inline int RandomInt(uint32_t& seed)
{
    seed = PCG_Hash(seed);
    return seed - INT32_MIN;
}

//Returns a random int between [min, max]
inline int RandomInt(uint32_t& seed, int min, int max)
{
    seed = PCG_Hash(seed);
    return min + (seed - INT32_MIN) % (max - min + 1);
}

//Returns a random float between [0, 1]
inline float RandomFloat(uint32_t& seed)
{
    seed = PCG_Hash(seed);

    return (float)seed / UINT32_MAX;
}

//Returns a random float between [min, max]
inline float RandomFloat(uint32_t& seed, float min, float max)
{
    seed = PCG_Hash(seed);

    float divisior = (float)UINT32_MAX / (max - min);

    return min + ((float)seed / divisior);
}

//Returns a random double between [0, 1]
inline double RandomDouble(uint32_t& seed)
{
    seed = PCG_Hash(seed);

    return (double)seed / UINT32_MAX;
}

//Returns a random dobule between [min, max]
inline float RandomDouble(uint32_t& seed, double min, double max)
{
    seed = PCG_Hash(seed);

    float divisior = (double)UINT32_MAX / (max - min);

    return min + ((double)seed / divisior);
}