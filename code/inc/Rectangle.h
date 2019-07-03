#pragma once

#include "Shape.h"



class Rectangle : public Shape
{
public:
    Rectangle(Rectangle& r) = default;
    Rectangle();
    Rectangle(std::string identifier, float rootCoordX, float rootCoordY);
    Rectangle(bool rotated, std::string identifier, float rootCoordX, float rootCoordY);
    Rectangle(float minWidth, float minHeight, std::string identifier, float rootCoordX, float rootCoordY);
    Rectangle(float minWidth, float minHeight, bool rotated, std::string identifier, float rootCoordX, float rootCoordY);

    float getMinWidth() const;
    float getMinHeight() const;
    bool getRotated() const;
    
    void setMinWidth(float input);
    void setMinHeight(float input);
    void setRotated(bool input);

protected:  
    float m_minWidth;
    float m_minHeight;
    bool m_rotated;
};
