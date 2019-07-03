#pragma once

#include "Shape.h"



class Circle : public Shape
{
public:
    Circle(Circle& c) = default;
    Circle();
    Circle(std::string identifier, float rootCoordX, float rootCoordY);
    Circle(float minSize, std::string identifier, float rootCoordX, float rootCoordY);

    float getMinSize() const;

    void setMinSize(float input);

protected:  
    float m_minSize;
};
