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

#include "State.h"
#include "States/States.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define FPS(amount) ((1000) / (amount))

typedef struct
{
    int32_t Time;
    int32_t Delta;
} Ticker;

/**
 * Returns milliseconds since last tick, representing true, else 0 if the ticker
 * hasn't ticked yet
 */
uint32_t TickerTick(Ticker *ticker)
{
    int32_t now = SDL_GetTicks();
    if (SDL_TICKS_PASSED(now, ticker->Time + ticker->Delta))
    {
        uint32_t origTime = ticker->Time;
        ticker->Time = now;
        return now - origTime;
    }

    return 0;
}

int32_t TickerTimeLeft(Ticker *ticker)
{
    return ((ticker->Time + ticker->Delta) - SDL_GetTicks());
}

int main(int argc, char *argv[])
{

    srand(time(NULL));

    App app = {.Window = NULL, .Renderer = NULL, .StateMachine = NULL};

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        perror("SDL_Init");
        SDL_Quit();
    }

    app.Window = SDL_CreateWindow("gamb", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                                  WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (!app.Window)
    {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
    }

    app.Renderer = SDL_CreateRenderer(app.Window, -1, SDL_RENDERER_ACCELERATED);
    if (!app.Renderer)
    {
        fprintf(stderr, "Renderer could not be created: %s\n", SDL_GetError());
        SDL_DestroyWindow(app.Window);
        SDL_Quit();
    }

    if (TTF_Init() == -1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to initialize TTF: %s", TTF_GetError());
        SDL_Quit();
    }

    // actual game stuff happens within the context of gameMachine, and nested
    // state machines
    app.StateMachine = StateMachineCreate(&app, &MenuState);

    SDL_Event ev;
    Input input;

    const int32_t startTime = SDL_GetTicks();

    // makes the collision calculations a bit more precise
    Ticker PhysicsTicker = {
        .Time = startTime,
        .Delta = FPS(120),
    };

    Ticker RenderTicker = {
        .Time = startTime,
        .Delta = FPS(60),
    };

    bool running = true;
    StateMachineStart(app.StateMachine);

    while (running)
    {

        uint32_t physicsTick = TickerTick(&PhysicsTicker);
        if (physicsTick)
        {
            while (SDL_PollEvent(&ev) != 0)
            {
                switch (ev.type)
                {
                case SDL_QUIT:
                    running = false;
                    break;
                }

                StateMachineHandleEvents(app.StateMachine, &ev);
            }

            input.KeyCodes = SDL_GetKeyboardState(NULL);
            StateMachineHandleInput(app.StateMachine, &input);

            float deltaTime = physicsTick / 1000.0f;

            StateMachineUpdate(app.StateMachine, deltaTime);
        }

        if (TickerTick(&RenderTicker))
        {
            SDL_SetRenderDrawColor(app.Renderer, PaletteBackground.r,
                                   PaletteBackground.g, PaletteBackground.b,
                                   PaletteBackground.a);

            SDL_RenderClear(app.Renderer);

            StateMachineDraw(app.StateMachine, app.Renderer);

            SDL_RenderPresent(app.Renderer);
        }

        // I'm sure there's a much better way of doing this, that's making
        // whoever read this later cry
        int32_t physicsTimeLeft = TickerTimeLeft(&PhysicsTicker);
        int32_t renderTimeLeft = TickerTimeLeft(&RenderTicker);
        int32_t timeLeft =
            renderTimeLeft < physicsTimeLeft ? renderTimeLeft : physicsTimeLeft;

        if (timeLeft > 0)
        {
            SDL_Delay(timeLeft); // don't wanna hog the CPU
        }
    }

    SDL_DestroyWindow(app.Window);

    return 0;
}
