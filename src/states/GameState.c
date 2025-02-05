#include "GameState.h"
#include "Collision.h"
#include "Consts.h"
#include "Math.h"
#include "Palette.h"
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

#define PLAYER_WIDTH 96
#define PLAYER_HEIGHT 8
#define PLAYER_PADDING 16

#define BALL_WIDTH 8
#define BALL_HEIGHT BALL_WIDTH
#define BALL_START_Y ((WINDOW_HEIGHT) - 64)

#define NROWS 8
#define NCOLS 12
#define NBLOCKS 96

#define BLOCKS_TOP_PADDING 48
#define BACKWALL_THICKNESS 4

#define INDEX(i, j) (((i) * (NCOLS)) + (j))

// Internal state machine
static StateMachine *GameStateMachine = NULL;

/**
 * Internal state machine state definitions
 */

// State functions
static void CountdownStateUpdate(float delta);
static void CountdownStateOnEnter();
static void CountdownStateDraw(SDL_Renderer *renderer);

static void PlayStateOnEnter();
static void PlayStateOnExit();
static void PlayStateUpdate(float delta);
static void PlayStateDraw(SDL_Renderer *renderer);
static void PlayStateHandleInput(const Input *input);

// Helper functions

static void BallReset();
static void DrawRect(SDL_Renderer *renderer, SDL_FRect *rect, SDL_Color *color);

static State CountdownState = {
    .OnEnter = CountdownStateOnEnter,
    .Update = CountdownStateUpdate,
    .Draw = CountdownStateDraw,
};

static State PlayState = {
    .OnEnter = PlayStateOnEnter,
    .Draw = PlayStateDraw,
    .HandleInput = PlayStateHandleInput,
    .Update = PlayStateUpdate,
    .OnExit = NULL,
    .HandleEvents = NULL,
};

// Game structs/definitions (Player, blocks, etc)

// TODO make label struct
typedef struct
{
    float Time;
    char Text[2];
    SDL_Color Color;
    SDL_FRect Rect;
    SDL_Texture *Texture;
    TTF_Font *Font;
} Countdown;

typedef struct
{
    uint32_t Score;
    char Text[16];
    SDL_Color Color;
    SDL_FRect Rect;
    SDL_Texture *Texture;
    TTF_Font *Font;
} Score;

typedef struct
{
    SDL_Color Color;
    SDL_FRect Rect;
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
    SDL_Color Color;
    SDL_FRect Rect;
    Vec2 Direction;
    float Speed;
} Ball;

typedef struct
{
    SDL_Color Color[NBLOCKS];
    SDL_FRect Rect[NBLOCKS];
    bool Alive[NBLOCKS];
    uint8_t Points[NBLOCKS];
    // size_t nblocks[NBLOCKS];
} Blocks;

typedef struct
{
    SDL_FRect Rect;
    SDL_Color Color;
} BackWall;

/**
 * Instances
 */

static Blocks blocks = {};

static Score score = {
    .Score = 0,
    .Rect = {.x = WINDOW_PADDING * 2, .y = WINDOW_PADDING * 2},
    .Color = PaletteForeground,
    .Text = "0",
};

static Countdown countdownTimer = {
    .Rect = {.x = (WINDOW_WIDTH - 50) / 2.0f,
             .y = (WINDOW_HEIGHT - 50) / 2.0f,
             .w = 50,
             .h = 50},
    .Color = PaletteForeground,
    .Time = 3.0f,
    .Text = "",

};

static Player player = {
    .MoveDirection = 0,
    .NormalSpeed = 500.0f,
    .FineTuneSpeed = 250.0f,
    .Speed = 0.0f,
    .Color = PaletteForeground,
    .SpeedLerp = 0.425f,
    .BoostSpeed = 5000.0f,
    .BoostCooldown = 0.0f,
    .Rect = {.x = (WINDOW_WIDTH - PLAYER_WIDTH) / 2.0f,
             .y = (WINDOW_HEIGHT - (PLAYER_HEIGHT + PLAYER_PADDING)),
             .w = PLAYER_WIDTH,
             .h = PLAYER_HEIGHT}};

static Ball ball = {.Speed = 500.0f,
                    .Color = PaletteForeground,
                    .Direction = {0.0, 0.0},
                    .Rect = {.x = (WINDOW_WIDTH - BALL_WIDTH) / 2.0f,
                             .y = BALL_START_Y,
                             .w = BALL_WIDTH,
                             .h = BALL_HEIGHT}};

// just a divider line, basically
static BackWall backWall = {
    .Color = PaletteForeground,
    .Rect = {.x = WINDOW_PADDING + BLOCK_PADDING,
             .y = BLOCKS_TOP_PADDING - BACKWALL_THICKNESS - WINDOW_PADDING,
             .w = WINDOW_WIDTH - WINDOW_PADDING * 2 - BLOCK_PADDING,
             .h = BACKWALL_THICKNESS},
};

static void CountdownStateOnEnter()
{
    BallReset();
    countdownTimer.Time = 3.0f;
}

static void CountdownStateUpdate(float delta)
{
    countdownTimer.Time -= delta;
    if (countdownTimer.Time <= 0.0f)
    {
        StateMachineTransitionTo(GameStateMachine, &PlayState);
    }
    snprintf(countdownTimer.Text, sizeof(countdownTimer.Text), "%d",
             (int)(ceilf(countdownTimer.Time)));
}

static void CountdownStateDraw(SDL_Renderer *renderer)
{
    DrawRect(renderer, &(player.Rect), &(player.Color));

    for (size_t i = 0; i < NBLOCKS; i++)
    {
        if (blocks.Alive[i])
        {
            DrawRect(renderer, &(blocks.Rect[i]), &(blocks.Color[i]));
        }
    }

    DrawRect(renderer, &(backWall.Rect), &(backWall.Color));

    // draw the text :(

    SDL_Surface *textSurface = TTF_RenderText_Blended(
        countdownTimer.Font, countdownTimer.Text, PaletteForeground);

    if (countdownTimer.Texture)
    {
        SDL_DestroyTexture(countdownTimer.Texture);
    }

    countdownTimer.Texture =
        SDL_CreateTextureFromSurface(renderer, textSurface);

    countdownTimer.Rect.x = (WINDOW_WIDTH - textSurface->w) / 2.0f;
    countdownTimer.Rect.y = (WINDOW_HEIGHT - textSurface->h) / 2.0f;
    countdownTimer.Rect.w = textSurface->w;
    countdownTimer.Rect.h = textSurface->h;

    SDL_FreeSurface(textSurface);

    SDL_SetRenderDrawColor(renderer, countdownTimer.Color.r,
                           countdownTimer.Color.g, countdownTimer.Color.b,
                           countdownTimer.Color.a);

    SDL_RenderCopyF(renderer, countdownTimer.Texture, NULL,
                    &(countdownTimer.Rect));
}

static void DrawRect(SDL_Renderer *renderer, SDL_FRect *rect, SDL_Color *color)
{
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    SDL_RenderFillRectF(renderer, rect);
}

static void BallReset()
{
    ball.Rect.x = (WINDOW_WIDTH - BALL_WIDTH) / 2.0f;
    ball.Rect.y = BALL_START_Y;
}

static void PlayStateOnEnter()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Entered PlayState");

    player.BoostCooldown = 0.0f;
    score.Score = 0;
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
        player.Color = PaletteLevel4;
    }
    else if (player.BoostCooldown >= 1.66f)
    {

        player.Color = PaletteLevel3;
    }
    else if (player.BoostCooldown >= 1.0f)
    {
        player.Color = PaletteLevel1; // skip the blue and just go to green
    }
    else
    {

        player.Color = PaletteForeground;
    }

    if (player.Rect.x < 0)
    {
        player.Rect.x = 0;
    }

    if ((player.Rect.x + player.Rect.w) > WINDOW_WIDTH)
    {
        player.Rect.x = WINDOW_WIDTH - player.Rect.w;
    }

    player.Rect.x += player.Speed * delta;

    if (ball.Rect.x < 0 || (ball.Rect.w + ball.Rect.x) > WINDOW_WIDTH)
    {
        ball.Direction.x = -ball.Direction.x;
    }

    if (ball.Rect.y < 0 || (ball.Rect.h + ball.Rect.y) > WINDOW_HEIGHT)
    {
        ball.Direction.y = -ball.Direction.y;
    }

    ball.Rect.x += ball.Direction.x * ball.Speed * delta;
    ball.Rect.y += ball.Direction.y * ball.Speed * delta;

    // now we calculate a new direction for the ball
    if (CheckCollision(ball.Rect, player.Rect) && ball.Direction.y > 0.0f)
    {
        float scale =
            (float)(ball.Rect.x - player.Rect.x) / (float)player.Rect.w;
        float angle = (-3 * M_PI / 4) + (M_PI / 2) * scale;

        ball.Direction = Vec2UnitFromAngle(angle);
    }

    for (size_t i = 0; i < NBLOCKS; i++)
    {
        if (blocks.Alive[i] && CheckCollision(ball.Rect, blocks.Rect[i]) &&
            ball.Direction.y < 0.0f)
        {
            score.Score += blocks.Points[i];
            snprintf(score.Text, sizeof(score.Text), "%d", score.Score);

            blocks.Alive[i] = false;

            float scale = (float)(ball.Rect.x - blocks.Rect[i].x) /
                          (float)blocks.Rect[i].w;
            float angle = (3 * M_PI / 4) - (M_PI / 2) * scale;
            ball.Direction = Vec2UnitFromAngle(angle);
        }
    }
}

static void PlayStateDraw(SDL_Renderer *renderer)
{
    DrawRect(renderer, &(player.Rect), &(player.Color));
    DrawRect(renderer, &(ball.Rect), &(ball.Color));

    for (size_t i = 0; i < NBLOCKS; i++)
    {
        if (blocks.Alive[i])
        {
            DrawRect(renderer, &(blocks.Rect[i]), &(blocks.Color[i]));
        }
    }

    DrawRect(renderer, &(backWall.Rect), &(backWall.Color));

    SDL_Surface *textSurface =
        TTF_RenderText_Blended(score.Font, score.Text, PaletteForeground);

    if (!textSurface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to get textSurface: %s", TTF_GetError());
    }

    if (score.Texture)
    {
        SDL_DestroyTexture(score.Texture);
    }

    score.Texture = SDL_CreateTextureFromSurface(renderer, textSurface);

    score.Rect.w = textSurface->w;
    score.Rect.h = textSurface->h;

    SDL_FreeSurface(textSurface);

    SDL_SetRenderDrawColor(renderer, score.Color.r, score.Color.g,
                           score.Color.b, score.Color.a);

    SDL_RenderCopyF(renderer, score.Texture, NULL, &(score.Rect));
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

/**
 * vvv External facing state to be passed to main state machine vvv
 */

static void GameStateOnEnter()
{

    // setting up the blocks, UI, etc

    countdownTimer.Font = TTF_OpenFont(GAME_FONT, 96);

    if (!countdownTimer.Font)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load font: %s",
                     TTF_GetError());
    }

    score.Font = TTF_OpenFont(GAME_FONT, 32);
    if (!score.Font)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load font: %s",
                     TTF_GetError());
    }

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

            blocks.Rect[i] = (SDL_FRect){
                .x = xpos, .y = ypos, .w = blockWidth, .h = blockHeight};

            blocks.Alive[i] = true;

            if (row < NROWS / 4)
            {
                blocks.Points[i] = 7;
                blocks.Color[i] = PaletteLevel4;
            }
            else if (row < NROWS / 2)
            {
                blocks.Points[i] = 5;
                blocks.Color[i] = PaletteLevel3;
            }
            else if (row < (3 * NROWS / 4))
            {
                blocks.Points[i] = 3;
                blocks.Color[i] = PaletteLevel2;
            }
            else
            {
                blocks.Points[i] = 1;
                blocks.Color[i] = PaletteLevel1;
            }
        }
    }

    GameStateMachine = StateMachineCreate(&CountdownState);
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

static void GameStateOnExit()
{
    StateMachineStop(GameStateMachine);

    if (countdownTimer.Texture)
    {
        SDL_DestroyTexture(countdownTimer.Texture);
    }

    if (countdownTimer.Font)
    {
        TTF_CloseFont(countdownTimer.Font);
    }
}

State GameState = {.OnEnter = GameStateOnEnter,
                   .OnExit = GameStateOnExit,
                   .Update = GameStateUpdate,
                   .Draw = GameStateDraw,
                   .HandleInput = GameStateHandleInput,
                   .HandleEvents = GameStateHandleEvents};
