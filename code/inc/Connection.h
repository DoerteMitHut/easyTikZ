#pragma once

#include <string>

class Connection
{
public:
    Connection(std::string identifierOrigin, std::string identifierTarget);
    Connection(std::string identifierOrigin, std::string identifierTarget, bool directional);

    std::string getIdentifierOrigin() const;
    std::string getIdentifierTarget() const;
    bool getDirectional() const;

    void setIdentifierOrigin(std::string input);
    void setIdentifierTarget(std::string input);
    void setDirectional(bool input);

protected:  
    std::string m_identifierOrigin;
    std::string m_identifierTarget;
    bool m_directional;
};
