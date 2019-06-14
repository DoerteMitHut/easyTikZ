#include "Rectangle.h"



//##### CONSTRUCTORS #####

Rectangle::Rectangle()
{
    setMinWidth(2);
    setMinHeight(1);
    setRotated(false);
    setIdentifier("null");
    setRootCoordX(0);
    setRootCoordY(0);
}

Rectangle::Rectangle(std::string identifier, float rootCoordX, float rootCoordY)
{
    setMinWidth(2);
    setMinHeight(1);
    setRotated(false);
    setIdentifier(identifier);
    setRootCoordX(rootCoordX);
    setRootCoordY(rootCoordY);
}

Rectangle::Rectangle(bool rotated, std::string identifier, float rootCoordX, float rootCoordY)
{
    setMinWidth(2);
    setMinHeight(1);
    setRotated(rotated);
    setIdentifier(identifier);
    setRootCoordX(rootCoordX);
    setRootCoordY(rootCoordY);
}

Rectangle::Rectangle(float minWidth, float minHeight, std::string identifier, float rootCoordX, float rootCoordY)
{
    setMinWidth(minWidth);
    setMinHeight(minHeight);
    setRotated(false);
    setIdentifier(identifier);
    setRootCoordX(rootCoordX);
    setRootCoordY(rootCoordY);
}

Rectangle::Rectangle(float minWidth, float minHeight, bool rotated, std::string identifier, float rootCoordX, float rootCoordY)
{
    setMinWidth(minWidth);
    setMinHeight(minHeight);
    setRotated(rotated);
    setIdentifier(identifier);
    setRootCoordX(rootCoordX);
    setRootCoordY(rootCoordY);
}



//##### GETTERS/SETTERS #####


float Rectangle::getMinWidth() const
{
    return this->m_minWidth;
}
float Rectangle::getMinHeight() const
{
    return this->m_minHeight;
}
bool Rectangle::getRotated() const
{
    return this->m_rotated;
}

void Rectangle::setMinWidth(float input)
{
    m_minWidth = input;
}
void Rectangle::setMinHeight(float input)
{
    m_minHeight = input;
}
void Rectangle::setRotated(bool input)
{
    m_rotated = input;
}
