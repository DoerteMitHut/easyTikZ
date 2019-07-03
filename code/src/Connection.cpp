#include "Connection.h"



//##### CONSTRUCTORS #####

Connection::Connection(std::string identifierOrigin, std::string identifierTarget)
{
    setIdentifierOrigin(identifierOrigin);
    setIdentifierTarget(identifierTarget);
    setDirectional(false);
}

Connection::Connection(std::string identifierOrigin, std::string identifierTarget, bool directional)
{
    setIdentifierOrigin(identifierOrigin);
    setIdentifierTarget(identifierTarget);
    setDirectional(directional);
}

Connection::Connection(std::string identifierOrigin, std::string identifierTarget, std::vector<std::pair<float, float>> intermediateCorners)
{
    setIdentifierOrigin(identifierOrigin);
    setIdentifierTarget(identifierTarget);
    setIntermediateCorners(intermediateCorners);
    setDirectional(false);
}

Connection::Connection(std::string identifierOrigin, std::string identifierTarget, std::vector<std::pair<float, float>> intermediateCorners, bool directional)
{
    setIdentifierOrigin(identifierOrigin);
    setIdentifierTarget(identifierTarget);
    setIntermediateCorners(intermediateCorners);
    setDirectional(directional);
}



//##### GETTERS/SETTERS #####

std::string Connection::getIdentifierOrigin() const
{
    return this->m_identifierOrigin;
}
std::string Connection::getIdentifierTarget() const
{
    return this->m_identifierTarget;
}
std::vector<std::pair<float,float>> Connection::getIntermediateCorners() const
{
    return this->m_intermediateCorners;
}
bool Connection::getDirectional() const
{
    return this->m_directional;
}

void Connection::setIdentifierOrigin(std::string input)
{
    m_identifierOrigin = input;
}
void Connection::setIdentifierTarget(std::string input)
{
    m_identifierTarget = input;
}
void Connection::setIntermediateCorners(std::vector<std::pair<float, float>> input)
{
    m_intermediateCorners.clear();
    m_intermediateCorners = input;
}
void Connection::addIntermediateCorner(std::pair<float,float> corner)
{
    m_intermediateCorners.push_back(corner);
}
void Connection::setDirectional(bool input)
{
    m_directional = input;
}
