#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Consts.h"
#include "Input.h"
#include "Palette.h"
#include "UI.h"

#include "states/GameState.h"
#include "states/State.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static StateMachine *gameMachine;

int main(int argc, char *argv[])
{
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        perror("SDL_Init");
        SDL_Quit();
    }

    window = SDL_CreateWindow("gamb", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                              WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window)
    {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        fprintf(stderr, "Renderer could not be created: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    if (TTF_Init() == -1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to initialize TTF: %s", TTF_GetError());
        SDL_Quit();
    }

    UiInitialize();

    // actual game stuff happens within the context of gameMachine, and nested
    // state machines
    gameMachine = StateMachineCreate(&GameState);

    SDL_Event ev;
    Input input;

    uint32_t lastPhysicsUpdate = SDL_GetTicks();
    bool running = true;

    StateMachineStart(gameMachine);
    while (running)
    {

        uint64_t frameStart = SDL_GetPerformanceCounter();

        while (SDL_PollEvent(&ev) != 0)
        {
            switch (ev.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            }

            StateMachineHandleEvents(gameMachine, &ev);
        }

        input.KeyCodes = SDL_GetKeyboardState(NULL);
        StateMachineHandleInput(gameMachine, &input);

        uint32_t currentPhysicsUpdate = SDL_GetTicks();
        float deltaTime = (currentPhysicsUpdate - lastPhysicsUpdate) / 1000.0f;

        StateMachineUpdate(gameMachine, deltaTime);

        lastPhysicsUpdate = currentPhysicsUpdate;

        SDL_SetRenderDrawColor(renderer, PaletteBackground.r,
                               PaletteBackground.g, PaletteBackground.b,
                               PaletteBackground.a);

        SDL_RenderClear(renderer);

        StateMachineDraw(gameMachine, renderer);

        SDL_RenderPresent(renderer);

        // cap to 60fps
        uint64_t frameEnd = SDL_GetPerformanceCounter();
        float elapsedMs = (frameEnd - frameStart) /
                          (float)SDL_GetPerformanceFrequency() * 1000.0f;
        SDL_Delay(floor(16.666f - elapsedMs));
    }

    SDL_DestroyWindow(window);

    return 0;
}
