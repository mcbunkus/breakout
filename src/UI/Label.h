#ifndef _GAMB_LABEL_H
#define _GAMB_LABEL_H

#include "Fonts/Font.h"
#include "Origin.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "UI.h"

typedef struct
{
    Widget base;
    SDL_Texture *Texture;
    TTF_Font *Font;
    SDL_Renderer *Renderer;
    Origin Origin;
    char Text[64]; // TODO make this not suck
} UiLabel;

UiLabel *UiLabelNew(SDL_Renderer *renderer, const char *text, const Font *font,
                    int fontsize, Origin origin, SDL_Color color);

UiLabel *UiLabelNewAtXY(SDL_Renderer *renderer, const char *text,
                        const Font *font, int fontsize, float x, float y,
                        Origin origin, SDL_Color color);

void UiLabelSetText(UiLabel *label, const char *format, ...);

#endif // !_GAMB_LABEL_H
