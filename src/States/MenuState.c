#include "MenuState.h"
#include "Button.h"
#include "Fonts/Square.h"
#include "Palette.h"

static UiButton PlayButton = {
    .Rectangle = {.Bounds = {.x = 100, .y = 100, .w = 256, .h = 32}},
    .IsPressed = false,
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
}

static void Draw(SDL_Renderer *renderer)
{
    UiButtonDraw(&PlayButton, renderer);
}

static void Enter(App *app)
{
    PlayButton.Label = LabelInit(app->Renderer, "Play", &FontSquare, 32, 120,
                                 120, LabelTopLeft, PaletteBackground);
}

State MenuState = {
    .Enter = Enter,
    .HandleEvents = HandleEvents,
    .Draw = Draw,
};
