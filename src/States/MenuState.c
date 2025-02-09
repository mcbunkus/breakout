#include "Consts.h"
#include "Fonts/Square.h"
#include "Palette.h"
#include "State.h"
#include "States.h"
#include "UI/Button.h"
#include "UI/Label.h"
#include "UI/UI.h"
#include "Util.h"

#define BUTTON_WIDTH 128
#define BUTTON_HEIGHT 48
#define BUTTON_PADDING 8

#define BUTTON_X ((WINDOW_WIDTH - BUTTON_WIDTH) / 2.0f)
#define PLAY_BUTTON_Y ((WINDOW_HEIGHT - BUTTON_HEIGHT - BUTTON_PADDING) / 2.0f)
#define QUIT_BUTTON_Y ((WINDOW_HEIGHT + BUTTON_HEIGHT + BUTTON_PADDING) / 2.0f)

static App *app = NULL;

static UiButtonState normal = {
    .Color = PaletteLevel1,
    .TextColor = PaletteBackground,
};

static UiButtonState hover = {
    .Color = PaletteLevel2,
    .TextColor = PaletteBackground,
};

static UiButtonState pressed = {
    .Color = PaletteLevel3,
    .TextColor = PaletteBackground,
};

static UiLabel *Title;
static UiButton *PlayButton;
static UiButton *QuitButton;

static void HandleEvents(SDL_Event *ev)
{

    WIDGET_HANDLE_EVENT(PlayButton, ev);
    WIDGET_HANDLE_EVENT(QuitButton, ev);

    if (PlayButton->IsReleased)
    {
        StateMachineTransitionTo(app->StateMachine, &GameState);
    }

    else if (QuitButton->IsReleased)
    {
        Exit();
    }
}

static void Draw(SDL_Renderer *renderer)
{

    WIDGET_DRAW(Title, renderer);
    WIDGET_DRAW(PlayButton, renderer);
    WIDGET_DRAW(QuitButton, renderer);
}

static void MenuStateExit(App *_app)
{
    WIDGET_DESTROY(Title);
    WIDGET_DESTROY(PlayButton);
    WIDGET_DESTROY(QuitButton);
}

static void Enter(App *_app)
{
    if (app == NULL)
    {
        app = _app;
    }

    Title = UiLabelNewAtXY(app->Renderer, "BREAKOUT", &FontSquare, 96,
                           WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 3.0,
                           OriginCenter, PaletteForeground);

    UiLabel *playLabel = UiLabelNew(app->Renderer, "PLAY", &FontSquare, 32,
                                    OriginCenter, PaletteBackground);
    UiLabel *quitLabel = UiLabelNew(app->Renderer, "QUIT", &FontSquare, 32,
                                    OriginCenter, PaletteBackground);

    PlayButton = UiButtonNew(playLabel, BUTTON_X, PLAY_BUTTON_Y, BUTTON_WIDTH,
                             BUTTON_HEIGHT, normal, hover, pressed);

    QuitButton = UiButtonNew(quitLabel, BUTTON_X, QUIT_BUTTON_Y, BUTTON_WIDTH,
                             BUTTON_HEIGHT, normal, hover, pressed);
}

State MenuState = {
    .Enter = Enter,
    .Exit = MenuStateExit,
    .HandleEvents = HandleEvents,
    .Draw = Draw,
};
