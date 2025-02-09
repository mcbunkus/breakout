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
            btn->IsReleased = false;
            break;
        case SDL_MOUSEBUTTONUP:
            btn->CurrentState = UiButtonStatesHovered;
            btn->IsPressed = false;
            btn->IsReleased = true;
            break;
        default:
            btn->CurrentState = UiButtonStatesHovered;
            break;
        }
    }
    else
    {
        btn->CurrentState = UiButtonStatesNormal;
        btn->IsReleased = false;
        btn->IsPressed = false;
    }
}

void UiButtonDraw(UiButton *btn, SDL_Renderer *renderer)
{
    // btn->Label.Rect.x = (int)btn->Rectangle.Bounds.x;
    // btn->Label.Rect.y = (int)btn->Rectangle.Bounds.y;
    btn->Rectangle.Color = btn->States[btn->CurrentState].Color;
    btn->Label.Color = btn->States[btn->CurrentState].TextColor;

    switch (btn->Label.Origin)
    {

    case OriginTopLeft:
        btn->Label.Rect.x = (int)btn->Rectangle.Bounds.x;
        btn->Label.Rect.y = (int)btn->Rectangle.Bounds.y;
        break;
    case OriginTopCenter:
        btn->Label.Rect.x =
            (int)btn->Rectangle.Bounds.x + btn->Rectangle.Bounds.w / 2;
        btn->Label.Rect.y = (int)btn->Rectangle.Bounds.y;
        break;
    case OriginTopRight:
        btn->Label.Rect.x =
            (int)btn->Rectangle.Bounds.x + btn->Rectangle.Bounds.w;
        btn->Label.Rect.y = (int)btn->Rectangle.Bounds.y;
        break;
    case OriginCenterLeft:
        btn->Label.Rect.x = (int)btn->Rectangle.Bounds.x;
        btn->Label.Rect.y =
            (int)btn->Rectangle.Bounds.y + btn->Rectangle.Bounds.h / 2;
        break;
    case OriginCenter:
        btn->Label.Rect.x =
            (int)btn->Rectangle.Bounds.x + btn->Rectangle.Bounds.w / 2;
        btn->Label.Rect.y =
            (int)btn->Rectangle.Bounds.y + btn->Rectangle.Bounds.h / 2;
        break;
    case OriginCenterRight:
        btn->Label.Rect.x =
            (int)btn->Rectangle.Bounds.x + btn->Rectangle.Bounds.w;
        btn->Label.Rect.y =
            (int)btn->Rectangle.Bounds.y + btn->Rectangle.Bounds.h / 2;
        break;
    case OriginBottomLeft:
        btn->Label.Rect.x = (int)btn->Rectangle.Bounds.x;
        btn->Label.Rect.y =
            (int)btn->Rectangle.Bounds.y + btn->Rectangle.Bounds.h;
        break;
    case OriginBottomCenter:
        btn->Label.Rect.x =
            (int)btn->Rectangle.Bounds.x + btn->Rectangle.Bounds.w / 2;
        btn->Label.Rect.y =
            (int)btn->Rectangle.Bounds.y + btn->Rectangle.Bounds.h;
        break;
    case OriginBottomRight:
        btn->Label.Rect.x =
            (int)btn->Rectangle.Bounds.x + btn->Rectangle.Bounds.w;
        btn->Label.Rect.y =
            (int)btn->Rectangle.Bounds.y + btn->Rectangle.Bounds.h;
        break;
    }

    RectangleDraw(&(btn->Rectangle), renderer);
    UiLabelDrawToRenderer(&(btn->Label), renderer);
}
