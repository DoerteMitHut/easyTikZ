#pragma once

#include <string>

class Shape
{
public:
    const std::string getIdentifier();
    const std::string getLabel();
    float getRootCoordX() const;
    float getRootCoordY() const;

    void setIdentifier(std::string input);
    void setLabel(std::string input);
    void setRootCoordX(float input);
    void setRootCoordY(float input);

private:
    std::string m_identifier;
    std::string m_label;
    float m_rootCoordX;
    float m_rootCoordY;
};

