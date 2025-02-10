#ifndef _GAMB_STATE_H
#define _GAMB_STATE_H

#include "Input.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>

typedef struct StateMachine StateMachine;
typedef struct App App;

struct App
{
    SDL_Window *Window;
    SDL_Renderer *Renderer;
    StateMachine *StateMachine;
};

typedef struct
{
    void (*Enter)(App *app);
    void (*Exit)(App *app);
    void (*Update)(float delta);
    void (*Draw)(SDL_Renderer *renderer);
    void (*HandleInput)(const Input *inputs);
    void (*HandleEvents)(SDL_Event *ev);
} State;

StateMachine *StateMachineCreate(App *app, State *initialState);
void StateMachineStart(StateMachine *machine);
void StateMachineStop(StateMachine *machine);

void StateMachineTransitionTo(StateMachine *machine, State *state);
bool StateMachineStateCurrentlyIs(StateMachine *machine, State *state);

void StateMachineHandleEvents(StateMachine *machine, SDL_Event *ev);
void StateMachineHandleInput(StateMachine *machine, const Input *input);
void StateMachineUpdate(StateMachine *machine, float delta);
void StateMachineDraw(StateMachine *machine, SDL_Renderer *renderer);

#endif // !_GAMB_STATE_H
