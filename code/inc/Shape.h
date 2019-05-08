#ifndef SHAPE_H
#define SHAPE_H

#include <string>

class Shape
{
public:
    virtual std::string toTikZ() = 0;
    float getRootCoordX();
    float getRootCoordY();
    void setRootCoordX(float input);
    void setRootCoordY(float input);

private:
    float m_rootCoordX;
    float m_rootCoordY;
};
#endif
