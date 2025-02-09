#include "Label.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <stdarg.h>
#include <string.h>

UiLabel UiLabelNew(SDL_Renderer *renderer, const char *text, const Font *font,
                   int fontsize, Origin origin, SDL_Color color)
{
    return UiLabelNewAtXY(renderer, text, font, fontsize, 0, 0, origin, color);
}

UiLabel UiLabelNewAtXY(SDL_Renderer *renderer, const char *text,
                       const Font *font, int fontsize, float x, float y,
                       Origin origin, SDL_Color color)
{

    SDL_RWops *fontMem = SDL_RWFromConstMem(font->Data, font->Len);

    if (!fontMem)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to read font data for Label: %s", SDL_GetError());
        return (UiLabel){};
    }

    TTF_Font *loadedFont = TTF_OpenFontRW(fontMem, 1, fontsize);

    if (!loadedFont)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to load font for Label: %s", TTF_GetError());
        return (UiLabel){};
    }

    UiLabel label = {.Renderer = renderer,
                     .Font = loadedFont,
                     .Color = color,
                     .Origin = origin};

    SDL_Surface *surface =
        TTF_RenderText_Blended(label.Font, text, label.Color);

    if (!surface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to get surface while creating label: %s",
                     TTF_GetError());
        return (UiLabel){};
    }

    label.Texture = SDL_CreateTextureFromSurface(renderer, surface);
    label.Rect.w = surface->w;
    label.Rect.h = surface->h;
    label.Rect.x = x;
    label.Rect.y = y;

    SDL_FreeSurface(surface);
    return label;
}

void UiLabelDraw(UiLabel *label)
{
    UiLabelDrawToRenderer(label, label->Renderer);
}

void UiLabelDrawToRenderer(UiLabel *label, SDL_Renderer *renderer)
{
    SDL_FRect destRect = {.w = label->Rect.w, .h = label->Rect.h};
    switch (label->Origin)
    {
    case OriginTopLeft:
        destRect.x = label->Rect.x;
        destRect.y = label->Rect.y;
        break;
    case OriginTopCenter:
        destRect.x = label->Rect.x - (label->Rect.w / 2.0f);
        destRect.y = label->Rect.y;
        break;
    case OriginTopRight:
        destRect.x = label->Rect.x - label->Rect.w;
        destRect.y = label->Rect.y;
        break;
    case OriginCenterLeft:
        destRect.x = label->Rect.x;
        destRect.y = label->Rect.y - (label->Rect.h / 2.0f);
        break;
    case OriginCenter:
        destRect.x = label->Rect.x - (label->Rect.w / 2.0f);
        destRect.y = label->Rect.y - (label->Rect.h / 2.0f);
        break;
    case OriginCenterRight:
        destRect.x = label->Rect.x - label->Rect.w;
        destRect.y = label->Rect.y - (label->Rect.h / 2.0f);
        break;
    case OriginBottomLeft:
        destRect.x = label->Rect.x;
        destRect.y = label->Rect.y - label->Rect.h;
        break;
    case OriginBottomCenter:
        destRect.x = label->Rect.x - (label->Rect.w / 2.0f);
        destRect.y = label->Rect.y - label->Rect.h;
        break;
    case OriginBottomRight:
        destRect.x = label->Rect.x - label->Rect.w;
        destRect.y = label->Rect.y - label->Rect.h;
        break;
    }
    SDL_SetRenderDrawColor(renderer, label->Color.r, label->Color.g,
                           label->Color.b, label->Color.a);

    SDL_RenderCopyF(renderer, label->Texture, NULL, &destRect);
}

void UiLabelSetText(UiLabel *label, const char *format, ...)
{
    if (!label)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Tried to set text on NULL Label");
    }

    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (strncmp(label->Text, buffer, sizeof(buffer)) == 0)
    {
        return; // same text, don't bother re-rendering
    }

    SDL_Surface *surface =
        TTF_RenderText_Blended(label->Font, buffer, label->Color);

    if (!surface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to render text to surface in label: %s",
                     TTF_GetError());
        return;
    }

    if (label->Texture)
    {
        SDL_DestroyTexture(label->Texture);
    }

    label->Texture = SDL_CreateTextureFromSurface(label->Renderer, surface);
    if (!label->Texture)
    {
        SDL_FreeSurface(surface);

        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to create texture from surface in Label: %s",
                     SDL_GetError());
        return;
    }

    label->Rect.w = surface->w;
    label->Rect.h = surface->h;

    SDL_FreeSurface(surface);
}

void UiLabelDestroy(UiLabel *label)
{
    if (label->Texture)
    {
        SDL_DestroyTexture(label->Texture);
    }
    if (label->Font)
    {
        TTF_CloseFont(label->Font);
    }
}
