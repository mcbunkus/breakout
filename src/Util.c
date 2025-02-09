#include "Util.h"
#include <SDL2/SDL.h>

void Exit()
{
    SDL_Event ev;
    ev.type = SDL_QUIT;
    SDL_PushEvent(&ev);
}
