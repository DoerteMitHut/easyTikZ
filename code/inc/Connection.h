#pragma once

#include <vector>
#include <string>
#include <utility>

class Connection
{
public:
    Connection(){};
    Connection(std::string identifierOrigin, std::string identifierTarget);
    Connection(std::string identifierOrigin, std::string identifierTarget, bool directional);
    Connection(std::string identifierOrigin, std::string identifierTarget, std::vector<std::pair<float, float>> intermediateCorners);
    Connection(std::string identifierOrigin, std::string identifierTarget, std::vector<std::pair<float, float>> intermediateCorners, bool directional);

    std::string getIdentifierOrigin() const;
    std::string getIdentifierTarget() const;
    std::vector<std::pair<float, float>> getIntermediateCorners() const;
    bool getDirectional() const;

    void setIdentifierOrigin(std::string input);
    void setIdentifierTarget(std::string input);
    void setIntermediateCorners(std::vector<std::pair<float, float>>);
    void addIntermediateCorner(std::pair<float,float> corner);
    void setDirectional(bool input);

protected:  
    std::string m_identifierOrigin;
    std::string m_identifierTarget;
    std::vector<std::pair<float, float>> m_intermediateCorners;
    bool m_directional;
};
