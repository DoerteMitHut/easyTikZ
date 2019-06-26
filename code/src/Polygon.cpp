#include "Polygon.h"



//##### CONSTRUCTORS #####

Polygon::Polygon()
{
    setMinSize(2);
    setPolySides(3);
    setIdentifier("null");
    setRootCoordX(0);
    setRootCoordY(0);
}

Polygon::Polygon(int polySides, std::string identifier, float rootCoordX, float rootCoordY)
{
    setMinSize(2);
    setPolySides(polySides);
    setIdentifier(identifier);
    setRootCoordX(rootCoordX);
    setRootCoordY(rootCoordY);
}

Polygon::Polygon(float minSize, int polySides, std::string identifier, float rootCoordX, float rootCoordY)
{
    setMinSize(minSize);
    setPolySides(polySides);
    setIdentifier(identifier);
    setRootCoordX(rootCoordX);
    setRootCoordY(rootCoordY);
}



//##### GETTERS/SETTERS #####


float Polygon::getMinSize() const
{
    return this->m_minSize;
}
int Polygon::getPolySides() const
{
    return this->m_polySides;
}

void Polygon::setMinSize(float input)
{
    m_minSize = input;
}
void Polygon::setPolySides(int input)
{
    m_polySides = input;
}
