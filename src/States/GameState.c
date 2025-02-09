#include "GameState.h"
#include "Consts.h"
#include "Fonts/Square.h"
#include "Label.h"
#include "Math.h"
#include "Palette.h"
#include "Rectangle.h"
#include "State.h"

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <stdint.h>
#include <stdio.h>

#include <SDL2/SDL.h>
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
    UiLabel Label;
} Countdown;

typedef struct
{
    uint32_t Score;
    UiLabel Label;
} Score;

typedef struct
{
    uint8_t LivesLeft;

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
    size_t BlockIndex;
    State State;
} GameReloadState;

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
static void CountdownStateEnter();
static void CountdownStateDraw(SDL_Renderer *renderer);

static void PlayStateEnter();
static void PlayStateEnter();
static void PlayStateUpdate(float delta);
static void PlayStateDraw(SDL_Renderer *renderer);
static void PlayStateHandleInput(const Input *input);

static void ReloadBlocksStateEnter();
static void ReloadBlocksStateDraw(SDL_Renderer *renderer);
static void ReloadBlocksStateUpdate(float delta);

//
//
//
// Helper functions
//
//
//
static void BallReset();
static void PlayerReset();

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

//
//
//
// Game state instances
//
//
//

static Blocks blocks = {};

static Score score = {
    .Score = 0,
    .Label = {},
};

static Countdown countdown = {
    .Label = {},
    .Time = 3.0f,

};

static Player player = {
    .MoveDirection = 0,
    .NormalSpeed = 1000.0f,
    .FineTuneSpeed = 500.0f,
    .Speed = 0.0f,
    .SpeedLerp = 0.25f,
    .BoostSpeed = 7500.0f,
    .BoostCooldown = 0.0f,
    .Rect = {.Color = PaletteForeground,
             .Bounds = {.x = (WINDOW_WIDTH - PLAYER_WIDTH) / 2.0f,
                        .y = (WINDOW_HEIGHT - (PLAYER_HEIGHT + PLAYER_PADDING)),
                        .w = PLAYER_WIDTH,
                        .h = PLAYER_HEIGHT}}};

static Ball ball = {.Speed = BALL_INITIAL_SPEED,
                    .Direction = {0.0, 0.0},
                    .Rect = {.Color = PaletteForeground,
                             .Bounds = {.x = (WINDOW_WIDTH - BALL_WIDTH) / 2.0f,
                                        .y = BALL_START_Y,
                                        .w = BALL_WIDTH,
                                        .h = BALL_HEIGHT}}};

// just a divider line, basically
static BackWall backWall = {
    .Rect = {
        .Color = PaletteForeground,
        .Bounds = {.x = WINDOW_PADDING + BLOCK_PADDING / 2.0f,
                   .y =
                       BLOCKS_TOP_PADDING - BACKWALL_THICKNESS - WINDOW_PADDING,
                   .w = WINDOW_WIDTH - WINDOW_PADDING * 2 - BLOCK_PADDING,
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

static void PlayerReset()
{
    player.Rect.Bounds.x = (WINDOW_WIDTH - PLAYER_WIDTH) / 2.0f;
}

//
//
// Reload state functions
//
//

static void ReloadBlocksStateEnter() { ReloadState.BlockIndex = 0; }

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

    for (size_t i = 0; i < NBLOCKS; i++)
    {
        if (blocks.Alive[i])
        {
            RectangleDraw(&(blocks.Rect[i]), renderer);
        }
    }

    RectangleDraw(&(backWall.Rect), renderer);
    LabelDraw(&(score.Label));
}

//
//
// Countdown state functions
//
//

static void CountdownStateEnter()
{
    BallReset();
    PlayerReset();
    countdown.Time = 3.0f;
}

static void CountdownStateUpdate(float delta)
{
    countdown.Time -= delta;
    if (countdown.Time <= 0.0f)
    {
        StateMachineTransitionTo(GameStateMachine, &PlayState);
    }

    LabelSetText(&(countdown.Label), "%d", (int)(ceilf(countdown.Time)));
}

static void CountdownStateDraw(SDL_Renderer *renderer)
{
    RectangleDraw(&(player.Rect), renderer);

    for (size_t i = 0; i < NBLOCKS; i++)
    {
        if (blocks.Alive[i])
        {
            RectangleDraw(&(blocks.Rect[i]), renderer);
        }
    }

    RectangleDraw(&(backWall.Rect), renderer);

    LabelDraw(&(countdown.Label));
}

//
//
//
// Play state functions
//
//
//

static void PlayStateEnter()
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
    else
    {
        player.Rect.Color = PaletteForeground;
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

    if (ball.Rect.Bounds.x < 0 ||
        (ball.Rect.Bounds.w + ball.Rect.Bounds.x) > WINDOW_WIDTH)
    {
        ball.Direction.x = -ball.Direction.x;
    }

    if (ball.Rect.Bounds.y < 0 ||
        (ball.Rect.Bounds.h + ball.Rect.Bounds.y) > WINDOW_HEIGHT)
    {
        ball.Direction.y = -ball.Direction.y;
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

    if (RectangleCheckCollision(&(ball.Rect), &(backWall.Rect)) &&
        ball.Direction.y < 0.0f)
    {
        ball.Direction.y = -ball.Direction.y;
    }

    for (size_t i = 0; i < NBLOCKS; i++)
    {
        if (blocks.Alive[i] &&
            RectangleCheckCollision(&(ball.Rect), &(blocks.Rect[i])) &&
            ball.Direction.y < 0.0f)
        {
            score.Score += blocks.Points[i];
            LabelSetText(&(score.Label), "%d", score.Score);

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

    for (size_t i = 0; i < NBLOCKS; i++)
    {
        if (blocks.Alive[i])
        {
            RectangleDraw(&(blocks.Rect[i]), renderer);
        }
    }

    RectangleDraw(&(backWall.Rect), renderer);

    LabelDraw(&(score.Label));
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
        float targetSpeed = input->KeyCodes[SDL_SCANCODE_LSHIFT]
                                ? player.FineTuneSpeed
                                : player.NormalSpeed;
        player.Speed = Flerp(player.Speed, targetSpeed * player.MoveDirection,
                             player.SpeedLerp);
    }

    // stops drifting
    if (fabsf(player.Speed) < 10)
    {
        player.Speed = 0;
    }
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

    countdown.Label =
        LabelInit(app->Renderer, "3", &FontSquare, 96, WINDOW_WIDTH / 2.0f,
                  WINDOW_HEIGHT / 2.0f, LabelCenter, PaletteForeground);

    score.Label = LabelInit(app->Renderer, "3", &FontSquare, 32,
                            WINDOW_PADDING * 2 - BLOCK_PADDING / 2.0f,
                            WINDOW_PADDING * 2 - BLOCK_PADDING / 2.0f,
                            LabelTopLeft, PaletteForeground);

    player.BoostCooldown = 0.0f;
    score.Score = 0;
    ball.Speed = BALL_INITIAL_SPEED;

    const int blockWidth =
        ((WINDOW_WIDTH - WINDOW_PADDING * 2) / NCOLS) - BLOCK_PADDING;

    const int blockHeight = 12;

    for (size_t row = 0; row < NROWS; row++)
    {
        for (size_t col = 0; col < NCOLS; col++)
        {
            size_t i = INDEX(row, col);
            int xpos = WINDOW_PADDING + (BLOCK_PADDING + blockWidth) * col +
                       BLOCK_PADDING / 2;
            int ypos = WINDOW_PADDING + BLOCKS_TOP_PADDING +
                       (BLOCK_PADDING + blockHeight) * row + BLOCK_PADDING / 2;

            blocks.Rect[i] = (Rectangle){
                .Bounds = {
                    .x = xpos, .y = ypos, .w = blockWidth, .h = blockHeight}};

            blocks.Alive[i] = true;

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

    GameStateMachine = StateMachineCreate(app, &CountdownState);
    StateMachineStart(GameStateMachine);
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

static void GameStateExit(App *app)
{
    StateMachineStop(GameStateMachine);
    LabelDestroy(&(countdown.Label));
    LabelDestroy(&(score.Label));
}

State GameState = {.Enter = GameStateEnter,
                   .Exit = GameStateExit,
                   .Update = GameStateUpdate,
                   .Draw = GameStateDraw,
                   .HandleInput = GameStateHandleInput,
                   .HandleEvents = GameStateHandleEvents};
