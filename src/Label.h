#ifndef _GAMB_LABEL_H
#define _GAMB_LABEL_H

#include "Fonts/Font.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef enum
{
    LabelTopLeft,
    LabelTopCenter,
    LabelTopRight,
    LabelCenterLeft,
    LabelCenter,
    LabelCenterRight,
    LabelBottomLeft,
    LabelBottomCenter,
    LabelBottomRight,
} LabelOrigin;

typedef struct
{
    SDL_Color Color;
    SDL_Rect Rect;
    SDL_Texture *Texture;
    TTF_Font *Font;
    SDL_Renderer *Renderer;
    LabelOrigin Origin;
    char Text[512];
} UiLabel;

UiLabel LabelInit(SDL_Renderer *renderer, const char *text, const Font *font,
                  int fontsize, float x, float y, LabelOrigin origin,
                  SDL_Color color);

void LabelDraw(UiLabel *label);
void LabelDrawToRenderer(UiLabel *label, SDL_Renderer *renderer);

void LabelSetText(UiLabel *label, const char *format, ...);
void LabelDestroy(UiLabel *label);

#endif // !_GAMB_LABEL_H
