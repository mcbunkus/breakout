#include "Button.h"
#include "Rectangle.h"
#include "UI/Label.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>

static void UiButtonHandleEvents(Widget *self, SDL_Event *event)
{

    UiButton *btn = (UiButton *)self;

    if (event->type != SDL_MOUSEMOTION && event->type != SDL_MOUSEBUTTONDOWN &&
        event->type != SDL_MOUSEBUTTONUP)
    {
        return;
    }

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    Point mouse = {.x = mouseX, .y = mouseY};
    Rectangle bounds = {
        .Bounds = {
            .x = self->X, .y = self->Y, .w = self->Width, .h = self->Height}};

    if (RectangleContains(&bounds, mouse))
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

static void UiButtonDraw(Widget *self, SDL_Renderer *renderer)
{

    UiButton *btn = (UiButton *)self;

    // btn->Label.Rect.x = (int)btn->Rectangle.Bounds.x;
    // btn->Label.Rect.y = (int)btn->Rectangle.Bounds.y;
    self->Color = btn->States[btn->CurrentState].Color;
    btn->Label->base.Color = btn->States[btn->CurrentState].TextColor;

    switch (btn->Label->Origin)
    {

    case OriginTopLeft:
        btn->Label->base.X = (int)self->X;
        btn->Label->base.Y = (int)self->Y;
        break;
    case OriginTopCenter:
        btn->Label->base.X = (int)self->X + self->Width / 2;
        btn->Label->base.Y = (int)self->Y;
        break;
    case OriginTopRight:
        btn->Label->base.X = (int)self->X + self->Width;
        btn->Label->base.Y = (int)self->Y;
        break;
    case OriginCenterLeft:
        btn->Label->base.X = (int)self->X;
        btn->Label->base.Y = (int)self->Y + self->Height / 2;
        break;
    case OriginCenter:
        btn->Label->base.X = (int)self->X + self->Width / 2;
        btn->Label->base.Y = (int)self->Y + self->Height / 2;
        break;
    case OriginCenterRight:
        btn->Label->base.X = (int)self->X + self->Width;
        btn->Label->base.Y = (int)self->Y + self->Height / 2;
        break;
    case OriginBottomLeft:
        btn->Label->base.X = (int)self->X;
        btn->Label->base.Y = (int)self->Y + self->Height;
        break;
    case OriginBottomCenter:
        btn->Label->base.X = (int)self->X + self->Width / 2;
        btn->Label->base.Y = (int)self->Y + self->Height;
        break;
    case OriginBottomRight:
        btn->Label->base.X = (int)self->X + self->Width;
        btn->Label->base.Y = (int)self->Y + self->Height;
        break;
    }

    Rectangle background = {
        .Bounds =
            {
                .x = self->X,
                .y = self->Y,
                .w = self->Width,
                .h = self->Height,
            },
        .Color = self->Color,
    };

    RectangleDraw(&background, renderer);
    WIDGET_DRAW(btn->Label, renderer);
}

void UiButtonDestroy(Widget *self)
{
    UiButton *btn = (UiButton *)self;
    WIDGET_DESTROY(btn->Label);
}

UiButton *UiButtonNew(UiLabel *label, float x, float y, UiButtonState normal,
                      UiButtonState hover, UiButtonState pressed)
{
    UiButton *button = malloc(sizeof(UiButton));

    button->Label = label;
    button->CurrentState = UiButtonStatesNormal;
    button->States[UiButtonStatesNormal] = normal;
    button->States[UiButtonStatesHovered] = hover;
    button->States[UiButtonStatesPressed] = pressed;
    button->Padding = 4.0f;

    button->base.X = x;
    button->base.Y = y;
    button->base.Width = label->base.Width + (2.0 * button->Padding);
    button->base.Height = label->base.Height + (2.0 * button->Padding);
    button->base.Color = normal.Color;

    button->base.Draw = UiButtonDraw;
    button->base.HandleEvent = UiButtonHandleEvents;
    button->base.Destroy = UiButtonDestroy;

    return button;
}
