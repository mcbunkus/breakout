#include "State.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include <stdlib.h>

struct StateMachine
{
    State *CurrentState;
};

StateMachine *StateMachineCreate(State *initialState)
{
    StateMachine *machine = malloc(sizeof(StateMachine));
    if (!machine)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to allocate StateMachine");
        return NULL;
    }

    if (!initialState)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "State passed to StateMachineInit was NULL!");
        return NULL;
    }

    machine->CurrentState = initialState;
    return machine;
}

void StateMachineStart(StateMachine *machine)
{
    if (machine->CurrentState && machine->CurrentState->OnEnter)
    {
        machine->CurrentState->OnEnter();
    }
}

void StateMachineStop(StateMachine *machine)
{
    if (machine->CurrentState && machine->CurrentState->OnExit)
    {
        machine->CurrentState->OnExit();
    }
}

void StateMachineTransitionTo(StateMachine *machine, State *state)
{
    if (!state)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Tried to set null State in StateMachine");
        SDL_Quit();
    }

    if (machine->CurrentState && machine->CurrentState->OnExit)
    {
        machine->CurrentState->OnExit();
    }

    machine->CurrentState = state;

    if (machine->CurrentState->OnEnter)
    {
        machine->CurrentState->OnEnter();
    }
}

bool StateMachineStateCurrentlyIs(StateMachine *machine, State *state)
{
    return state == machine->CurrentState;
}

void StateMachineHandleEvents(StateMachine *machine, SDL_Event *ev)
{
    if (machine->CurrentState->HandleEvents)
    {
        machine->CurrentState->HandleEvents(ev);
    }
}

void StateMachineUpdate(StateMachine *machine, float delta)
{
    if (machine->CurrentState->Update)
    {
        machine->CurrentState->Update(delta);
    }
}

void StateMachineDraw(StateMachine *machine, SDL_Renderer *renderer)
{
    if (machine->CurrentState->Draw)
    {
        machine->CurrentState->Draw(renderer);
    }
}

void StateMachineHandleInput(StateMachine *machine, const Input *input)
{
    if (machine->CurrentState->HandleInput)
    {
        machine->CurrentState->HandleInput(input);
    }
}
