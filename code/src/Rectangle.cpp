#include "Rectangle.h"



//##### CONSTRUCTORS #####

Rectangle::Rectangle()
{
    setMinWidth(2);
    setMinHeight(1);
    setIdentifier("null");
    setRootCoordX(0);
    setRootCoordY(0);
}

Rectangle::Rectangle(std::string identifier, float rootCoordX, float rootCoordY)
{
    setMinWidth(2);
    setMinHeight(1);
    setIdentifier(identifier);
    setRootCoordX(rootCoordX);
    setRootCoordY(rootCoordY);
}

Rectangle::Rectangle(float minWidth, float minHeight, std::string identifier, float rootCoordX, float rootCoordY)
{
    setMinWidth(minWidth);
    setMinHeight(minHeight);
    setIdentifier(identifier);
    setRootCoordX(rootCoordX);
    setRootCoordY(rootCoordY);
}



//##### GETTERS/SETTERS #####


float Rectangle::getMinWidth()
{
    return this->m_minWidth;
}
float Rectangle::getMinHeight()
{
    return this->m_minHeight;
}

void Rectangle::setMinWidth(float input)
{
    m_minWidth = input;
}
void Rectangle::setMinHeight(float input)
{
    m_minHeight = input;
}
