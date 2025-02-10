#include "UI.h"

#include <SDL2/SDL_log.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

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

void ContainerAddWidget(Container *container, Widget *widget)
{
    if (container->NumChildren >= container->Capacity)
    {
        int newCapacity =
            container->Capacity == 0 ? 4 : container->Capacity * 2;
        Widget **newChildren =
            realloc(container->Children, newCapacity * sizeof(Widget *));

        if (!newChildren)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Failed to add widget to container: %s",
                         strerror(errno));
            return;
        }
        container->Children = newChildren;
        container->Capacity = newCapacity;
    }
}

void ContainerDestroy(Widget *widget)
{
    if (!widget)
    {
        return;
    }

    Container *self = (Container *)widget;

    for (int i = 0; i < self->NumChildren; i++)
    {
        WIDGET_DESTROY(self->Children[i]);
    }
}
