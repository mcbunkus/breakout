#ifndef _GAMB_LABEL_H
#define _GAMB_LABEL_H

#include "Fonts/Font.h"
#include "UI.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct
{
    SDL_Color Color;
    SDL_Rect Rect;
    SDL_Texture *Texture;
    TTF_Font *Font;
    SDL_Renderer *Renderer;
    Origin Origin;
    char Text[512];
} UiLabel;

UiLabel UiLabelInit(SDL_Renderer *renderer, const char *text, const Font *font,
                    int fontsize, float x, float y, Origin origin,
                    SDL_Color color);

void UiLabelDraw(UiLabel *label);
void UiLabelDrawToRenderer(UiLabel *label, SDL_Renderer *renderer);

void UiLabelSetText(UiLabel *label, const char *format, ...);
void UiLabelDestroy(UiLabel *label);

#endif // !_GAMB_LABEL_H
