#include "Consts.h"
#include "Fonts/Square.h"
#include "Palette.h"
#include "State.h"
#include "States.h"
#include "UI/Button.h"
#include "UI/UI.h"

#define BUTTON_WIDTH 128
#define BUTTON_HEIGHT 64

static App *app = NULL;

static UiButton PlayButton = {
    .IsPressed = false,
    .Rectangle = {.Bounds = {.x = 100, .y = 100, .w = 256, .h = 32}},
    .CurrentState = UiButtonStatesNormal,
    .States = {
        [UiButtonStatesNormal] = {.Color = PaletteLevel1,
                                  .TextColor = PaletteBackground},
        [UiButtonStatesHovered] = {.Color = PaletteLevel2,
                                   .TextColor = PaletteBackground},
        [UiButtonStatesPressed] = {.Color = PaletteLevel3,
                                   .TextColor = PaletteBackground},
    }};

static void HandleEvents(SDL_Event *ev)
{
    UiButtonHandleEvents(&PlayButton, ev);

    if (PlayButton.IsPressed)
    {
        StateMachineTransitionTo(app->StateMachine, &GameState);
    }
}

static void Draw(SDL_Renderer *renderer)
{
    UiButtonDraw(&PlayButton, renderer);
}

static void Enter(App *_app)
{

    if (app == NULL)
    {
        app = _app;
    }

    PlayButton.Label = UiLabelInit(app->Renderer, "Play", &FontSquare, 32, 120,
                                   120, OriginCenterLeft, PaletteBackground);
}

State MenuState = {
    .Enter = Enter,
    .HandleEvents = HandleEvents,
    .Draw = Draw,
};
