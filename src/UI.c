#include "UI.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>

TTF_Font *Font;

void UiInitialize()
{
    Font = TTF_OpenFont("assets/AldotheApache.ttf", 48);
    if (!Font)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to load game font: %s", TTF_GetError());
        SDL_Quit();
    }
}
