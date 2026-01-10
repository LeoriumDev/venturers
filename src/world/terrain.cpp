#include <cmath>

#include "terrain.hpp"

constexpr unsigned int seed = 123456789u;

unsigned int hash(unsigned int x);
float fade(float t);
float grad(int p0);
float noise(float p);

unsigned int hash(unsigned int x) {
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

float fade(const float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float grad(const int p0) {
    const unsigned int h = hash(static_cast<unsigned int>(p0) ^ seed);
    return (h & 1u) ? 1.0f : -1.0f;
}

float noise(const float p) {
    const float p0f = std::floor(p);
    const float p1f = p0f + 1.0f;
    const int p0 = static_cast<int>(p0f);
    const int p1 = p0 + 1;

    const float t = p - p0f;
    const float fade_t = fade(t);

    const float g0 = grad(p0);
    const float g1 = grad(p1);

    return (1.0f - fade_t) * g0 * (p - p0f) + fade_t * g1 * (p - p1f);
}

int terrainHeight(const int worldX, const int worldZ) {
    constexpr float frequency = 0.02f;
    constexpr float amplitude = 10.0f;
    return static_cast<int>((noise(worldX * frequency) + noise(worldZ * frequency)) * amplitude);
}

