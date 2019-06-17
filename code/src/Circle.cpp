#include "Circle.h"



//##### CONSTRUCTORS #####

Circle::Circle()
{
    setMinSize(2);
    setIdentifier("null");
    setRootCoordX(0);
    setRootCoordY(0);
}

Circle::Circle(std::string identifier, float rootCoordX, float rootCoordY)
{
    setMinSize(2);
    setIdentifier(identifier);
    setRootCoordX(rootCoordX);
    setRootCoordY(rootCoordY);
}

Circle::Circle(float minSize, std::string identifier, float rootCoordX, float rootCoordY)
{
    setMinSize(minSize);
    setIdentifier(identifier);
    setRootCoordX(rootCoordX);
    setRootCoordY(rootCoordY);
}



//##### GETTERS/SETTERS #####


float Circle::getMinSize() const
{
    return this->m_minSize;
}

void Circle::setMinSize(float input)
{
    m_minSize = input;
}
