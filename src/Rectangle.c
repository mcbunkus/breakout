#include "Rectangle.h"

void RectangleDraw(Rectangle *rect, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, rect->Color.r, rect->Color.g,
                           rect->Color.b, rect->Color.a);
    SDL_RenderFillRectF(renderer, &(rect->Bounds));
}

bool RectangleCheckCollision(const Rectangle *a, const Rectangle *b)
{
    return a->Bounds.x + a->Bounds.w >= b->Bounds.x &&
           a->Bounds.x <= b->Bounds.x + b->Bounds.w &&
           a->Bounds.y + a->Bounds.h >= b->Bounds.y &&
           a->Bounds.y <= b->Bounds.y + b->Bounds.h;
}
