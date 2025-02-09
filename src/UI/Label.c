#include "Label.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <stdarg.h>
#include <string.h>

static void UiLabelDraw(Widget *self, SDL_Renderer *renderer)
{
    UiLabel *label = (UiLabel *)self;

    SDL_FRect destRect = {.w = self->Width, .h = self->Height};
    switch (label->Origin)
    {
    case OriginTopLeft:
        destRect.x = self->X;
        destRect.y = self->Y;
        break;
    case OriginTopCenter:
        destRect.x = self->X - (self->Width / 2.0f);
        destRect.y = self->Y;
        break;
    case OriginTopRight:
        destRect.x = self->X - self->Width;
        destRect.y = self->Y;
        break;
    case OriginCenterLeft:
        destRect.x = self->X;
        destRect.y = self->Y - (self->Height / 2.0f);
        break;
    case OriginCenter:
        destRect.x = self->X - (self->Width / 2.0f);
        destRect.y = self->Y - (self->Height / 2.0f);
        break;
    case OriginCenterRight:
        destRect.x = self->X - self->Width;
        destRect.y = self->Y - (self->Height / 2.0f);
        break;
    case OriginBottomLeft:
        destRect.x = self->X;
        destRect.y = self->Y - self->Height;
        break;
    case OriginBottomCenter:
        destRect.x = self->X - (self->Width / 2.0f);
        destRect.y = self->Y - self->Height;
        break;
    case OriginBottomRight:
        destRect.x = self->X - self->Width;
        destRect.y = self->Y - self->Height;
        break;
    }

    SDL_SetRenderDrawColor(renderer, self->Color.r, self->Color.g,
                           self->Color.b, self->Color.a);

    SDL_RenderCopyF(renderer, label->Texture, NULL, &destRect);
}

static void UiLabelDestroy(Widget *self)
{

    UiLabel *label = (UiLabel *)self;

    if (!label)
    {
        return;
    }

    if (label->Texture)
    {
        SDL_DestroyTexture(label->Texture);
    }
    if (label->Font)
    {
        TTF_CloseFont(label->Font);
    }
}

UiLabel *UiLabelNew(SDL_Renderer *renderer, const char *text, const Font *font,
                    int fontsize, Origin origin, SDL_Color color)
{
    return UiLabelNewAtXY(renderer, text, font, fontsize, 0, 0, origin, color);
}

UiLabel *UiLabelNewAtXY(SDL_Renderer *renderer, const char *text,
                        const Font *font, int fontsize, float x, float y,
                        Origin origin, SDL_Color color)
{

    UiLabel *label = malloc(sizeof(UiLabel));

    SDL_RWops *fontMem = SDL_RWFromConstMem(font->Data, font->Len);

    if (!fontMem)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to read font data for Label: %s", SDL_GetError());
        return NULL;
    }

    TTF_Font *loadedFont = TTF_OpenFontRW(fontMem, 1, fontsize);

    if (!loadedFont)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to load font for Label: %s", TTF_GetError());
        return NULL;
    }

    SDL_Surface *surface = TTF_RenderText_Blended(loadedFont, text, color);

    if (!surface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to get surface while creating label: %s",
                     TTF_GetError());
        return NULL;
    }

    label->base.Draw = UiLabelDraw;
    label->base.Destroy = UiLabelDestroy;
    label->base.Width = surface->w;
    label->base.Height = surface->h;
    label->base.Color = color;
    label->base.X = x;
    label->base.Y = y;

    label->Renderer = renderer;
    label->Font = loadedFont;
    label->Origin = origin;
    label->Texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);
    return label;
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
        TTF_RenderText_Blended(label->Font, buffer, label->base.Color);

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

    label->base.Width = surface->w;
    label->base.Height = surface->h;

    SDL_FreeSurface(surface);
}
