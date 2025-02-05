#ifndef _GAMB_VEC2_H
#define _GAMB_VEC2_H

#include <SDL2/SDL_log.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    float x, y;
} Vec2;

static inline Vec2 Vec2New(float x, float y) { return (Vec2){.x = x, .y = y}; }
static inline Vec2 Vec2Zero() { return (Vec2){.x = 0, .y = 0}; }
static inline Vec2 Vec2Add(Vec2 a, Vec2 b)
{
    return Vec2New(a.x + b.x, a.y + b.y);
}

static inline Vec2 Vec2Subtract(Vec2 a, Vec2 b)
{
    return Vec2New(a.x - b.x, a.y - b.y);
}

static inline float Vec2Dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }

static inline Vec2 Vec2Scale(Vec2 v, float scalar)
{
    return Vec2New(v.x * scalar, v.y * scalar);
}

static inline float Vec2LengthSquared(Vec2 v) { return v.x * v.x + v.y * v.y; }
static inline float Vec2Length(Vec2 v) { return sqrtf(Vec2LengthSquared(v)); }

static inline Vec2 Vec2Normalized(Vec2 v)
{
    float len = Vec2Length(v);
    if (fabs(len) < 1e-5)
    {
        return Vec2Zero();
    }
    return Vec2Scale(v, 1.0f / len);
}

static inline Vec2 Vec2Rotated(Vec2 v, float angle)
{
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    return Vec2New(v.x * cosA - v.y * sinA, v.x * sinA + v.y * cosA);
}

static inline Vec2 Vec2Lerp(Vec2 from, Vec2 to, float t)
{
    return Vec2Add(Vec2Scale(from, 1.0f - t), Vec2Scale(to, t));
}

static inline float Vec2Distance(Vec2 a, Vec2 b)
{
    return Vec2Length(Vec2Subtract(a, b));
}

static inline void Vec2Print(Vec2 v) { printf("Vec2(%f, %f)\n", v.x, v.y); }

static Vec2 Vec2UnitFromAngle(float angle)
{
    return Vec2New(cosf(angle), sinf(angle));
}

static inline Vec2 Vec2RandomUnit(float min, float max)
{
    float scale = rand() / (float)RAND_MAX;
    float angle = min + scale * (max - min);
    return Vec2UnitFromAngle(angle);
}

static inline float Flerp(float from, float to, float t)
{
    return from + (to - from) * t;
}

static inline bool FApproxZero(float val, float eps)
{
    return fabsf(val) < eps;
}

#endif // !_GAMB_VEC2_H
