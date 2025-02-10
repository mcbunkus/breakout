#ifndef _GAMB_UI_BUTTON_H
#define _GAMB_UI_BUTTON_H

#include "Label.h"
#include "Palette.h"
// #include "Rectangle.h"
#include <stdbool.h>

typedef struct
{
    SDL_Color Color;
    SDL_Color TextColor;
} UiButtonState;

typedef enum
{
    UiButtonStatesNormal,
    UiButtonStatesHovered,
    UiButtonStatesPressed,
    UiButtonStatesLen
} UiButtonStatesEnum;

typedef struct
{
    Widget base;
    // Rectangle Rectangle;
    UiButtonState States[UiButtonStatesLen];
    UiButtonStatesEnum CurrentState;
    UiLabel *Label;
    float Padding; // default is 4px
    bool IsPressed;
    bool IsReleased;
} UiButton;

UiButton *UiButtonNew(UiLabel *label, float x, float y, UiButtonState normal,
                      UiButtonState hover, UiButtonState pressed);

// void UiButtonHandleEvents(UiButton *btn, SDL_Event *event);
// void UiButtonDraw(UiButton *btn, SDL_Renderer *renderer);

static UiButtonState DefaultNormal = {
    .Color = PaletteLevel1,
    .TextColor = PaletteBackground,
};

static UiButtonState DefaultHover = {
    .Color = PaletteLevel2,
    .TextColor = PaletteBackground,
};

static UiButtonState DefaultPressed = {
    .Color = PaletteLevel3,
    .TextColor = PaletteBackground,
};
#endif // !_GAMB_UI_BUTTON_H
