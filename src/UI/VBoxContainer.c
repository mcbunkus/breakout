#include "VBoxContainer.h"
#include "UI/UI.h"

VBoxContainer *VBoxContainerNew(float x, float y, float padding, float spacing)
{
    VBoxContainer *vbox = malloc(sizeof(VBoxContainer));
    vbox->Padding = padding;
    vbox->Spacing = spacing;

    Widget *widget = (Widget *)vbox;
    widget->X = x;
    widget->Y = y;
    widget->Destroy = ContainerDestroy;
    widget->HandleEvent = ContainerHandleEvent;

    return vbox;
}
