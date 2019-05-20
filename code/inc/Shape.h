#pragma once

#include <string>

class Shape
{
public:
    std::string getIdentifier();
    float getRootCoordX();
    float getRootCoordY();

    void setIdentifier(std::string input);
    void setRootCoordX(float input);
    void setRootCoordY(float input);

private:
    std::string m_identifier;
    float m_rootCoordX;
    float m_rootCoordY;
};

