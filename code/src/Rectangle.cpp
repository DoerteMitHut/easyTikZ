#include "Rectangle.h"

Rectangle::Rectangle()
{
    setRootCoordX(0);
    setRootCoordY(0);
}

Rectangle::Rectangle(float rootCoordX, float rootCoordY)
{
    setRootCoordX(rootCoordX);
    setRootCoordY(rootCoordY);
}

float Rectangle::getSize()
{
    return size;
}

int Rectangle::getVerts()
{
    return verts;
}

void Rectangle::setSize(float input)
{
    size = input;
}

void Rectangle::setVerts(int input)
{
    verts = input;
}
