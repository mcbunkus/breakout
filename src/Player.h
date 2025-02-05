#ifndef _GAMB_PLAYER_H
#define _GAMB_PLAYER_H

#include <SDL2/SDL.h>

#define PLAYER_WIDTH 96
#define PLAYER_HEIGHT 8

typedef struct
{
    SDL_Color Color;
    SDL_FRect Rect;
    float NormalSpeed;
    float FineTuneSpeed;
    float Speed;
    float SpeedLerp;
    int MoveDirection;
} Player;

#endif // !_GAMB_PLAYER_H
