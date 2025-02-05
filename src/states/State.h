#ifndef _GAMB_STATE_H
#define _GAMB_STATE_H

#include "Input.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>

typedef struct
{
    void (*OnEnter)(void);
    void (*OnExit)(void);
    void (*Update)(float delta);
    void (*Draw)(SDL_Renderer *renderer);
    void (*HandleInput)(const Input *renderer);
    void (*HandleEvents)(SDL_Event *ev);
} State;

typedef struct StateMachine StateMachine;

StateMachine *StateMachineCreate(State *initialState);
void StateMachineStart(StateMachine *machine);
void StateMachineStop(StateMachine *machine);

void StateMachineTransitionTo(StateMachine *machine, State *state);
bool StateMachineStateCurrentlyIs(StateMachine *machine, State *state);

void StateMachineHandleEvents(StateMachine *machine, SDL_Event *ev);
void StateMachineHandleInput(StateMachine *machine, const Input *input);
void StateMachineUpdate(StateMachine *machine, float delta);
void StateMachineDraw(StateMachine *machine, SDL_Renderer *renderer);

#endif // !_GAMB_STATE_H
