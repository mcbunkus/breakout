#pragma once

#include "UI.h"

typedef struct VBoxContainer
{
    Container base;
    float Padding; // defaults to 4px
    float Spacing; // defaults to 4px
} VBoxContainer;

VBoxContainer *VBoxContainerNew(float x, float y, float padding, float spacing);
