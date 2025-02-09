#include "UI.h"

void WidgetHandleEvent(Widget *self, SDL_Event *event)
{
    if (self && self->HandleEvent)
    {

        self->HandleEvent(self, event);
    }
}

void WidgetDraw(Widget *widget, SDL_Renderer *renderer)
{
    if (widget && widget->Draw)
    {
        widget->Draw(widget, renderer);
    }
}

void WidgetDestroy(Widget *widget)
{
    if (widget && widget->Destroy)
    {
        widget->Destroy(widget);
    }
}
