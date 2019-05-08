#ifndef SHAPE_H
#define SHAPE_H

#include <string>

class Shape
{
public:
    virtual std::string toTikZ() = 0;

    std::string getIdentifier();
    float getRootCoordX();
    float getRootCoordY();

    void setIdentifier(std::string input);
    void setRootCoordX(float input);
    void setRootCoordY(float input);

private:
    std::string identifier;
    float m_rootCoordX;
    float m_rootCoordY;
};
#endif
