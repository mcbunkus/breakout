#include <SDL2/SDL.h>

#include "State.h"
#include "States.h"

#include "Consts.h"
#include "Math.h"
#include "Palette.h"
#include "Rectangle.h"

#include "Fonts/Square.h"
#include "UI/Button.h"
#include "UI/Label.h"
#include "UI/UI.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define INDEX(i, j) (((i) * (NCOLS)) + (j))

//
//
//
// Game structs, such as the player, ball, blocks, score, etc
//
//
//

typedef struct
{
    float Time;
    UiLabel *Label;
} Countdown;

typedef struct
{
    uint32_t Score;
    UiLabel *Label;
} Score;

typedef struct
{
    uint8_t LivesLeft;
    Rectangle Indicators[];
} Lives;

typedef struct
{
    Rectangle Rect;
    float NormalSpeed;
    float FineTuneSpeed;
    float BoostSpeed;
    float BoostCooldown;
    float Speed;
    float SpeedLerp;
    int MoveDirection;
} Player;

typedef struct
{
    Rectangle Rect;
    Vec2 Direction;
    float Speed;
} Ball;

typedef struct
{
    Rectangle Rect[NBLOCKS];
    bool Alive[NBLOCKS];
    uint8_t Points[NBLOCKS];
} Blocks;

typedef struct
{
    Rectangle Rect;
} BackWall;

typedef struct
{
    State State;
    size_t BlockIndex;
} GameReloadState;

typedef struct
{
    State State;
    App *App;
    UiButton *PlayAgainButton;
    UiButton *BackToMenuButton;
    float BlockDelayTimer;
    float BlockDelayTime;
} GameOverState;

//
//
//
// Internal state machine and renderer, used for loading fonts and stuff
//
//
//

static StateMachine *GameStateMachine = NULL;

//
//
//
// Internal state machine function declarations
//
//
//

static void CountdownStateUpdate(float delta);
static void CountdownStateEnter(App *app);
static void CountdownStateDraw(SDL_Renderer *renderer);

static void PlayStateEnter(App *app);
static void PlayStateEnter(App *app);
static void PlayStateUpdate(float delta);
static void PlayStateDraw(SDL_Renderer *renderer);
static void PlayStateHandleInput(const Input *input);

static void ReloadBlocksStateEnter(App *app);
static void ReloadBlocksStateDraw(SDL_Renderer *renderer);
static void ReloadBlocksStateUpdate(float delta);

static void GameOverStateEnter(App *app);
static void GameOverStateDraw(SDL_Renderer *renderer);
static void GameOverStateUpdate(float delta);
static void GameOverStateHandleEvents(SDL_Event *ev);

//
//
//
// Helper functions
//
//
//
static void BallReset();

//
//
//
// Game States
//
//
//

static State CountdownState = {
    .Enter = CountdownStateEnter,
    .Update = CountdownStateUpdate,
    .Draw = CountdownStateDraw,
};

static State PlayState = {
    .Enter = PlayStateEnter,
    .Draw = PlayStateDraw,
    .HandleInput = PlayStateHandleInput,
    .Update = PlayStateUpdate,
    .Exit = NULL,
    .HandleEvents = NULL,
};

// static State GameOverState = {
//     .Enter = GameOverStateEnter,
//     .Update = GameOverStateUpdate,
//     .Draw = GameOverStateDraw,
// };

static GameReloadState ReloadState = {

    .BlockIndex = 0, // only needed in the reload state context
    .State =
        {
            .Enter = ReloadBlocksStateEnter,
            .Update = ReloadBlocksStateUpdate,

            // we can reuse this function, drawing
            // all the same stuff with nothing new
            .Draw = ReloadBlocksStateDraw,

        },
};

static GameOverState GameOver = {

    .State =
        {
            .Draw = GameOverStateDraw,
            .HandleEvents = GameOverStateHandleEvents,
            .Update = GameOverStateUpdate,
            .Enter = GameOverStateEnter,
        },
};

//
//
//
// Game state instances
//
//
//

static Lives lives = {
    .LivesLeft = NUM_LIVES,
    .Indicators =
        {
            {
                .Color = PaletteForeground,
                .Bounds = {.x = WINDOW_WIDTH - WINDOW_PADDING -
                                LIVES_INDICATOR_DIM,
                           .y = WINDOW_PADDING,
                           .w = LIVES_INDICATOR_DIM,
                           .h = LIVES_INDICATOR_DIM},
            },
            {
                .Color = PaletteForeground,
                .Bounds = {.x = WINDOW_WIDTH - WINDOW_PADDING -
                                2.0f * LIVES_INDICATOR_DIM -
                                2.0f * LIVES_INDICATOR_PADDING,
                           .y = WINDOW_PADDING,
                           .w = LIVES_INDICATOR_DIM,
                           .h = LIVES_INDICATOR_DIM},
            },
            {
                .Color = PaletteForeground,
                .Bounds = {.x = WINDOW_WIDTH - WINDOW_PADDING -
                                3.0f * LIVES_INDICATOR_DIM -
                                4.0f * LIVES_INDICATOR_PADDING,
                           .y = WINDOW_PADDING,
                           .w = LIVES_INDICATOR_DIM,
                           .h = LIVES_INDICATOR_DIM},
            },
        },
};

static Blocks blocks = {};

static Score score = {
    .Score = 0,
    .Label = NULL,
};

static Countdown countdown = {
    .Time = 3.0f,
    .Label = NULL,

};

static Player player = {
    .MoveDirection = 0,
    .NormalSpeed = 1000.0f,
    .FineTuneSpeed = 500.0f,
    .Speed = 0.0f,
    .SpeedLerp = 0.25f,
    .BoostSpeed = 7500.0f,
    .BoostCooldown = 0.0f,
    .Rect =
        {
            .Color = PaletteForeground,
            .Bounds =
                {
                    .x = (WINDOW_WIDTH - PLAYER_WIDTH) / 2.0f,
                    .y = (WINDOW_HEIGHT - (PLAYER_HEIGHT + PLAYER_PADDING)),
                    .w = PLAYER_WIDTH,
                    .h = PLAYER_HEIGHT,
                },
        },
};

static Ball ball = {
    .Speed = BALL_INITIAL_SPEED,
    .Direction = {0.0, 0.0},
    .Rect =
        {
            .Color = PaletteForeground,
            .Bounds = {.x = (WINDOW_WIDTH - BALL_WIDTH) / 2.0f,
                       .y = BALL_START_Y,
                       .w = BALL_WIDTH,
                       .h = BALL_HEIGHT},
        },
};

// just a divider line, basically
static BackWall backWall = {
    .Rect = {
        .Color = PaletteForeground,
        .Bounds = {.x = WINDOW_PADDING,
                   .y =
                       BLOCKS_TOP_PADDING - BACKWALL_THICKNESS - WINDOW_PADDING,
                   .w = WINDOW_WIDTH - 2 * WINDOW_PADDING,
                   .h = BACKWALL_THICKNESS},
    }};

//
//
// Helper functions
//
//

static void BallReset()
{
    ball.Rect.Bounds.x = (WINDOW_WIDTH - BALL_WIDTH) / 2.0f;
    ball.Rect.Bounds.y = BALL_START_Y;
}

static void DrawLifeIndicators(SDL_Renderer *renderer)
{

    for (int i = 0; i < lives.LivesLeft; i++)
    {
        RectangleDraw(&(lives.Indicators[i]), renderer);
    }
}

static void DrawBlocks(SDL_Renderer *renderer)
{
    for (size_t i = 0; i < NBLOCKS; i++)
    {
        if (blocks.Alive[i])
        {
            RectangleDraw(&(blocks.Rect[i]), renderer);
        }
    }
}

static void ResetGame()
{
    score.Score = 0;
    UiLabelSetText(score.Label, "0");
    UiLabelSetText(countdown.Label, "0");

    player.BoostCooldown = 0.0f;
    lives.LivesLeft = NUM_LIVES;
    ball.Speed = BALL_INITIAL_SPEED;

    for (int i = 0; i < NBLOCKS; i++)
    {
        blocks.Alive[i] = false;
    }
}

//
//
// Reload state functions
//
//

static void ReloadBlocksStateEnter(App *app) { ReloadState.BlockIndex = 0; }

static void ReloadBlocksStateUpdate(float delta)
{
    static const float blockDelay = 0.01f;
    static float blockDelayCounter = blockDelay;

    blockDelayCounter -= delta;

    if (blockDelayCounter < 0.0f)
    {
        blocks.Alive[ReloadState.BlockIndex] = true;
        blockDelayCounter = blockDelay;
        ReloadState.BlockIndex += 1;
    }

    if (ReloadState.BlockIndex == NBLOCKS)
    {
        StateMachineTransitionTo(GameStateMachine, &CountdownState);
    }
}

static void ReloadBlocksStateDraw(SDL_Renderer *renderer)
{
    RectangleDraw(&(player.Rect), renderer);

    DrawBlocks(renderer);

    RectangleDraw(&(backWall.Rect), renderer);
    WIDGET_DRAW(score.Label, renderer);

    DrawLifeIndicators(renderer);
}

//
//
// Countdown state functions
//
//

static void CountdownStateEnter(App *_)
{
    BallReset();
    // PlayerReset();
    countdown.Time = 3.0f;
}

static void CountdownStateUpdate(float delta)
{
    countdown.Time -= delta;
    if (countdown.Time <= 0.0f)
    {
        StateMachineTransitionTo(GameStateMachine, &PlayState);
    }

    // lerp the player to the center while the countdown runs
    player.Rect.Bounds.x =
        Flerp(player.Rect.Bounds.x,
              (WINDOW_WIDTH - player.Rect.Bounds.w) / 2.0f, 0.075);

    UiLabelSetText(countdown.Label, "%d", (int)(ceilf(countdown.Time)));
}

static void CountdownStateDraw(SDL_Renderer *renderer)
{
    RectangleDraw(&(player.Rect), renderer);

    DrawBlocks(renderer);

    RectangleDraw(&(backWall.Rect), renderer);

    WIDGET_DRAW(countdown.Label, renderer);
    WIDGET_DRAW(score.Label, renderer);

    DrawLifeIndicators(renderer);
}

//
//
//
// Play state functions
//
//
//

static void PlayStateEnter(App *app)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Entered PlayState");
    ball.Direction = Vec2RandomUnit(-3 * M_PI / 4, -M_PI / 4);
}

static void PlayStateUpdate(float delta)
{

    if (player.BoostCooldown >= 0.0f)
    {
        player.BoostCooldown -= delta;
    }

    if (player.BoostCooldown >= 2.33f)
    {
        player.Rect.Color = PaletteLevel4;
    }
    else if (player.BoostCooldown >= 1.66f)
    {

        player.Rect.Color = PaletteLevel3;
    }
    else if (player.BoostCooldown >= 1.0f)
    {
        player.Rect.Color = PaletteLevel1; // skip the blue and just go to green
    }

    if (player.Rect.Bounds.x < 0)
    {
        player.Rect.Bounds.x = 0;
    }

    if ((player.Rect.Bounds.x + player.Rect.Bounds.w) > WINDOW_WIDTH)
    {
        player.Rect.Bounds.x = WINDOW_WIDTH - player.Rect.Bounds.w;
    }

    player.Rect.Bounds.x += player.Speed * delta;

    // Sometimes the ball gets kinda stuck in the wall if it hits it hard
    // enough, velocity checks are for making sure it reflects when it hits the
    // side of the window going in

    if ((ball.Rect.Bounds.x < 0 && ball.Direction.x < 0) ||
        ((ball.Rect.Bounds.w + ball.Rect.Bounds.x) > WINDOW_WIDTH) &&
            ball.Direction.x > 0)
    {
        ball.Direction.x = -ball.Direction.x;
    }

    // bounce off the backwall
    if (ball.Rect.Bounds.y <
            (backWall.Rect.Bounds.y + backWall.Rect.Bounds.h) &&
        ball.Direction.y < 0.0f)
    {
        ball.Direction.y = -ball.Direction.y;
    }

    // went below the screen, player lost a life
    if ((ball.Rect.Bounds.h + ball.Rect.Bounds.y) > WINDOW_HEIGHT)
    {
        lives.LivesLeft -= 1;
        if (lives.LivesLeft <= 0)
        {
            StateMachineTransitionTo(GameStateMachine, &(GameOver.State));
        }
        else
        {
            StateMachineTransitionTo(GameStateMachine, &CountdownState);
        }
    }

    ball.Rect.Bounds.x += ball.Direction.x * ball.Speed * delta;
    ball.Rect.Bounds.y += ball.Direction.y * ball.Speed * delta;

    // now we calculate a new direction for the ball
    if (RectangleCheckCollision(&(ball.Rect), &(player.Rect)) &&
        ball.Direction.y > 0.0f)
    {
        float scale = (float)(ball.Rect.Bounds.x - player.Rect.Bounds.x) /
                      (float)player.Rect.Bounds.w;
        float angle = (-3 * M_PI / 4) + (M_PI / 2) * scale;

        ball.Direction = Vec2UnitFromAngle(angle);
    }

    for (size_t i = 0; i < NBLOCKS; i++)
    {
        if (blocks.Alive[i] &&
            RectangleCheckCollision(&(ball.Rect), &(blocks.Rect[i])) &&
            ball.Direction.y < 0.0f)
        {
            score.Score += blocks.Points[i];
            UiLabelSetText(score.Label, "%d", score.Score);

            blocks.Alive[i] = false;

            float scale =
                (float)(ball.Rect.Bounds.x - blocks.Rect[i].Bounds.x) /
                (float)blocks.Rect[i].Bounds.w;

            float angle = (3 * M_PI / 4) - (M_PI / 2) * scale;
            ball.Direction = Vec2UnitFromAngle(angle);

            ball.Speed += blocks.Points[i];
        }
    }

    bool noBlocksLeft = true;
    for (size_t i = 0; i < NBLOCKS; i++)
    {
        if (blocks.Alive[i])
        {
            noBlocksLeft = false;
            break;
        }
    }

    if (noBlocksLeft)
    {
        StateMachineTransitionTo(GameStateMachine, &(ReloadState.State));
    }
}

static void PlayStateDraw(SDL_Renderer *renderer)
{
    RectangleDraw(&(player.Rect), renderer);
    RectangleDraw(&(ball.Rect), renderer);

    DrawBlocks(renderer);

    RectangleDraw(&(backWall.Rect), renderer);
    WIDGET_DRAW(score.Label, renderer);

    DrawLifeIndicators(renderer);
}

static void PlayStateHandleInput(const Input *input)
{
    player.MoveDirection = 0;

    if (input->KeyCodes[SDL_SCANCODE_A])
    {
        player.MoveDirection -= 1;
    }
    if (input->KeyCodes[SDL_SCANCODE_D])
    {
        player.MoveDirection += 1;
    }

    if (player.MoveDirection != 0 && FApproxZero(player.BoostCooldown, 0.1) &&
        input->KeyCodes[SDL_SCANCODE_SPACE])
    {
        player.Speed = player.BoostSpeed * player.MoveDirection;
        player.BoostCooldown = 3.0f;
    }
    else
    {
        float targetSpeed = 0.0f;
        if (input->KeyCodes[SDL_SCANCODE_LSHIFT])
        {
            targetSpeed = player.FineTuneSpeed;
            player.Rect.Color = PaletteLevel2;
        }
        else
        {
            targetSpeed = player.NormalSpeed;
            player.Rect.Color = PaletteForeground;
        }
        player.Speed = Flerp(player.Speed, targetSpeed * player.MoveDirection,
                             player.SpeedLerp);
    }

    // stops drifting
    if (fabsf(player.Speed) < 10)
    {
        player.Speed = 0;
    }
}

static void GameOverStateEnter(App *app)
{
    GameOver.App = app;

    // reuse the countdown label for the score, since it's already there doing
    // nothing

    UiLabelSetText(countdown.Label, "%d", score.Score);
    SDL_Log("Entered Game Over State");
}

static void GameOverStateUpdate(float delta) {}

static void GameOverStateHandleEvents(SDL_Event *ev)
{
    WIDGET_HANDLE_EVENT(GameOver.PlayAgainButton, ev);
    WIDGET_HANDLE_EVENT(GameOver.BackToMenuButton, ev);

    if (GameOver.PlayAgainButton->IsPressed)
    {
        ResetGame();
        StateMachineTransitionTo(GameStateMachine, &ReloadState.State);
    }

    else if (GameOver.BackToMenuButton->IsPressed)
    {
        StateMachineTransitionTo(GameOver.App->StateMachine, &MenuState);
    }
}

static void GameOverStateDraw(SDL_Renderer *renderer)
{
    RectangleDraw(&(player.Rect), renderer);
    RectangleDraw(&(backWall.Rect), renderer);

    WIDGET_DRAW(countdown.Label, renderer);
    WIDGET_DRAW(GameOver.PlayAgainButton, renderer);
    WIDGET_DRAW(GameOver.BackToMenuButton, renderer);

    DrawBlocks(renderer);
}

static void GameOverStateExit(App *app)
{
    UiLabelSetText(countdown.Label, "0");
}

//
//
//
// Top-level game state functions
//
//
//

static void GameStateEnter(App *app)
{

    // setting up the blocks, UI, etc

    const float buttonY = 2.0f * WINDOW_HEIGHT / 3.0f;
    const float buttonWidth = 256.0f;

    countdown.Label =
        UiLabelNewAtXY(app->Renderer, "3", &FontSquare, 96, WINDOW_WIDTH / 2.0f,
                       WINDOW_HEIGHT / 2.0f, OriginCenter, PaletteForeground);

    score.Label =
        UiLabelNewAtXY(app->Renderer, "0", &FontSquare, 32, WINDOW_PADDING,
                       WINDOW_PADDING, OriginTopLeft, PaletteForeground);

    UiLabel *playAgainLabel =
        UiLabelNew(app->Renderer, "PLAY AGAIN", &FontSquare, 32, OriginCenter,
                   PaletteBackground);
    UiLabel *backToMenuLabel =
        UiLabelNew(app->Renderer, "BACK TO MENU", &FontSquare, 32, OriginCenter,
                   PaletteBackground);

    GameOver.PlayAgainButton = UiButtonNew(
        playAgainLabel, ((WINDOW_WIDTH - BUTTON_PADDING) / 2.0f) - buttonWidth,
        buttonY, DefaultNormal, DefaultHover, DefaultPressed);

    GameOver.BackToMenuButton =
        UiButtonNew(backToMenuLabel, ((WINDOW_WIDTH + BUTTON_PADDING) / 2.0f),
                    buttonY, DefaultNormal, DefaultHover, DefaultPressed);

    GameOver.PlayAgainButton->base.Width = buttonWidth;
    GameOver.BackToMenuButton->base.Width = buttonWidth;

    const float blockWidth = (WINDOW_WIDTH - (2.0 * WINDOW_PADDING) -
                              ((NCOLS - 1) * BLOCK_PADDING)) /
                             NCOLS;

    const float blockHeight = 12;

    for (size_t row = 0; row < NROWS; row++)
    {
        for (size_t col = 0; col < NCOLS; col++)
        {
            size_t i = INDEX(row, col);

            float xpos = WINDOW_PADDING + (blockWidth + BLOCK_PADDING) * col;
            float ypos = WINDOW_PADDING + BLOCKS_TOP_PADDING +
                         (blockHeight + BLOCK_PADDING) * row;

            blocks.Rect[i] = (Rectangle){
                .Bounds = {.x = xpos,
                           .y = ypos,
                           .w = blockWidth,
                           .h = blockHeight},
            };

            if (row < NROWS / 4)
            {
                blocks.Points[i] = 7;
                blocks.Rect[i].Color = PaletteLevel4;
            }
            else if (row < NROWS / 2)
            {
                blocks.Points[i] = 5;
                blocks.Rect[i].Color = PaletteLevel3;
            }
            else if (row < (3 * NROWS / 4))
            {
                blocks.Points[i] = 3;
                blocks.Rect[i].Color = PaletteLevel2;
            }
            else
            {
                blocks.Points[i] = 1;
                blocks.Rect[i].Color = PaletteLevel1;
            }
        }
    }

    // Initializes everything to its starting state
    ResetGame();

    // Creating the remaining tries indicators
    GameStateMachine = StateMachineCreate(app, &(ReloadState.State));
    StateMachineStart(GameStateMachine);
}

static void GameStateExit(App *app)
{
    StateMachineStop(GameStateMachine);
    WIDGET_DESTROY(countdown.Label);
    WIDGET_DESTROY(score.Label);
    WIDGET_DESTROY(GameOver.PlayAgainButton);
    WIDGET_DESTROY(GameOver.BackToMenuButton);
}

static void GameStateUpdate(float delta)
{
    StateMachineUpdate(GameStateMachine, delta);
}

static void GameStateDraw(SDL_Renderer *renderer)
{
    StateMachineDraw(GameStateMachine, renderer);
}
static void GameStateHandleInput(const Input *input)
{
    StateMachineHandleInput(GameStateMachine, input);
}

static void GameStateHandleEvents(SDL_Event *ev)
{
    StateMachineHandleEvents(GameStateMachine, ev);
}

State GameState = {.Enter = GameStateEnter,
                   .Exit = GameStateExit,
                   .Update = GameStateUpdate,
                   .Draw = GameStateDraw,
                   .HandleInput = GameStateHandleInput,
                   .HandleEvents = GameStateHandleEvents};
