#include "Rectangle.h"

Rectangle::Rectangle(std::string identifier, float rootCoordX, float rootCoordY)
{
    setIdentifier(identifier);
    setRootCoordX(rootCoordX);
    setRootCoordY(rootCoordY);
}

Rectangle::Rectangle(float minWidth, float minHeight, std::string identifier, float rootCoordX, float rootCoordY)
{
    setIdentifier(identifier);
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
