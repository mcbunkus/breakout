#ifndef _GAMB_RECTANGLE_H
#define _GAMB_RECTANGLE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct
{
    SDL_FRect Bounds;
    SDL_Color Color;
} Rectangle;

void RectangleDraw(Rectangle *rect, SDL_Renderer *renderer);
bool RectangleCheckCollision(const Rectangle *a, const Rectangle *b);

#endif // !_GAMB_RECTANGLE_H
