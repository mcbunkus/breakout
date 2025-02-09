#pragma once

#include <SDL2/SDL.h>

typedef struct Widget
{
    float X;
    float Y;
    float Width;
    float Height;
    SDL_Color Color;

    void (*Draw)(struct Widget *self, SDL_Renderer *renderer);
    void (*HandleEvent)(struct Widget *self, SDL_Event *event);
    void (*Destroy)(struct Widget *self);

} Widget;

void WidgetDraw(Widget *widget, SDL_Renderer *renderer);
void WidgetHandleEvent(Widget *widget, SDL_Event *event);
void WidgetDestroy(Widget *widget);

//
//
//
// Convenience macros so I don't forget to accidentally cast
//
//

#define WIDGET_DRAW(widget, renderer) WidgetDraw((Widget *)(widget), (renderer))
#define WIDGET_DESTROY(widget) WidgetDestroy((Widget *)(widget))

#define WIDGET_HANDLE_EVENT(widget, event)                                     \
    WidgetHandleEvent((Widget *)(widget), (event))
