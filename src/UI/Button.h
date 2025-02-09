#ifndef _GAMB_UI_BUTTON_H
#define _GAMB_UI_BUTTON_H

#include "Label.h"
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
    bool IsPressed;
    bool IsReleased;
} UiButton;

UiButton *UiButtonNew(UiLabel *label, float x, float y, float w, float h,
                      UiButtonState normal, UiButtonState hover,
                      UiButtonState pressed);

// void UiButtonHandleEvents(UiButton *btn, SDL_Event *event);
// void UiButtonDraw(UiButton *btn, SDL_Renderer *renderer);

#endif // !_GAMB_UI_BUTTON_H
