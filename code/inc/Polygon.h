#pragma once

#include "Shape.h"



class Polygon : public Shape
{
public:
    Polygon(Polygon& p)=default;
    Polygon();
    Polygon(int polySides, std::string identifier, float rootCoordX, float rootCoordY);
    Polygon(float minSize, int polySides, std::string identifier, float rootCoordX, float rootCoordY);

    float getMinSize() const;
    int getPolySides() const;

    void setMinSize(float input);
    void setPolySides(int input);

protected:  
    float m_minSize;
    int m_polySides;
};
