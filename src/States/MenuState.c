#include "Consts.h"
#include "Fonts/Square.h"
#include "Palette.h"
#include "State.h"
#include "States.h"
#include "UI/Button.h"
#include "Util.h"

#define BUTTON_WIDTH 128
#define BUTTON_HEIGHT 48
#define BUTTON_PADDING 8

#define BUTTON_X ((WINDOW_WIDTH - BUTTON_WIDTH) / 2.0f)
#define PLAY_BUTTON_Y ((WINDOW_HEIGHT - BUTTON_HEIGHT - BUTTON_PADDING) / 2.0f)
#define QUIT_BUTTON_Y ((WINDOW_HEIGHT + BUTTON_HEIGHT + BUTTON_PADDING) / 2.0f)

static App *app = NULL;

static const UiButtonState DefaultButtonStates[UiButtonStatesLen] = {};

static UiButton PlayButton;
static UiButton QuitButton;

static void HandleEvents(SDL_Event *ev)
{
    UiButtonHandleEvents(&PlayButton, ev);
    UiButtonHandleEvents(&QuitButton, ev);

    if (PlayButton.IsReleased)
    {
        StateMachineTransitionTo(app->StateMachine, &GameState);
    }

    else if (QuitButton.IsReleased)
    {
        Exit();
    }
}

static void Draw(SDL_Renderer *renderer)
{
    UiButtonDraw(&PlayButton, renderer);
    UiButtonDraw(&QuitButton, renderer);
}

static void Enter(App *_app)
{

    if (app == NULL)
    {
        app = _app;
    }

    PlayButton = (UiButton){
        .IsPressed = false,
        .CurrentState = UiButtonStatesNormal,
        .Rectangle = {.Bounds = {.x = BUTTON_X,
                                 .y = PLAY_BUTTON_Y,
                                 .w = BUTTON_WIDTH,
                                 .h = BUTTON_HEIGHT}},
        .Label = UiLabelNew(app->Renderer, "Play", &FontSquare, 32,
                            OriginCenter, PaletteBackground),
        .States = {
            [UiButtonStatesNormal] = {.Color = PaletteLevel1,
                                      .TextColor = PaletteBackground},
            [UiButtonStatesHovered] = {.Color = PaletteLevel2,
                                       .TextColor = PaletteBackground},
            [UiButtonStatesPressed] = {.Color = PaletteLevel3,
                                       .TextColor = PaletteBackground},

        }};

    QuitButton = (UiButton){
        .IsPressed = false,
        .CurrentState = UiButtonStatesNormal,
        .Rectangle = {.Bounds = {.x = BUTTON_X,
                                 .y = QUIT_BUTTON_Y,
                                 .w = BUTTON_WIDTH,
                                 .h = BUTTON_HEIGHT}},
        .Label = UiLabelNew(app->Renderer, "Quit", &FontSquare, 32,
                            OriginCenter, PaletteBackground),
        .States = {
            [UiButtonStatesNormal] = {.Color = PaletteLevel1,
                                      .TextColor = PaletteBackground},
            [UiButtonStatesHovered] = {.Color = PaletteLevel2,
                                       .TextColor = PaletteBackground},
            [UiButtonStatesPressed] = {.Color = PaletteLevel3,
                                       .TextColor = PaletteBackground},

        }};
}

State MenuState = {
    .Enter = Enter,
    .HandleEvents = HandleEvents,
    .Draw = Draw,
};
