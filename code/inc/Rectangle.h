#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Shape.h"

class Rectangle : public Shape
{
public:
    Rectangle(std::string identifier, float rootCoordX, float rootCoordY);
    Rectangle(float minWidth, float minHeight, std::string identifier, float rootCoordX, float rootCoordY);

    float getMinWidth();
    float getMinHeight();

    void setMinWidth(float input);
    void setMinHeight(float input);

protected:  
    float m_minWidth;
    float m_minHeight;
};
#endif
