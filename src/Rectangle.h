#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct
{
    float x;
    float y;
} Point;

typedef struct
{
    SDL_FRect Bounds;
    SDL_Color Color;
} Rectangle;

void RectangleDraw(const Rectangle *rect, SDL_Renderer *renderer);
bool RectangleCheckCollision(const Rectangle *a, const Rectangle *b);
bool RectangleContains(const Rectangle *rect, Point point);
