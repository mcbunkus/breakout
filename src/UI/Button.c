#include "Button.h"
#include "Rectangle.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>

void UiButtonHandleEvents(UiButton *btn, SDL_Event *event)
{
    if (event->type != SDL_MOUSEMOTION && event->type != SDL_MOUSEBUTTONDOWN &&
        event->type != SDL_MOUSEBUTTONUP)
    {
        return;
    }

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    Point mouse = {.x = mouseX, .y = mouseY};

    if (RectangleContains(&(btn->Rectangle), mouse))
    {

        switch (event->type)
        {
        case SDL_MOUSEBUTTONDOWN:
            btn->CurrentState = UiButtonStatesPressed;
            btn->IsPressed = true;
            break;
        case SDL_MOUSEBUTTONUP:
            btn->CurrentState = UiButtonStatesHovered;
            btn->IsPressed = false;
            break;
        default:
            btn->CurrentState = UiButtonStatesHovered;
            break;
        }
    }
    else
    {
        btn->CurrentState = UiButtonStatesNormal;
        btn->IsPressed = false;
    }
}

void UiButtonDraw(UiButton *btn, SDL_Renderer *renderer)
{
    btn->Rectangle.Color = btn->States[btn->CurrentState].Color;
    btn->Label.Color = btn->States[btn->CurrentState].TextColor;

    RectangleDraw(&(btn->Rectangle), renderer);
    UiLabelDrawToRenderer(&(btn->Label), renderer);
}
