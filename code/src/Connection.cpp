#include "Connection.h"



//##### CONSTRUCTORS #####

Connection::Connection(std::string identifierOrigin, std::string identifierTarget)
{
    setIdentifierOrigin(identifierOrigin);
    setIdentifierTarget(identifierTarget);
}

Connection::Connection(std::string identifierOrigin, std::string identifierTarget, bool directional)
{
    setIdentifierOrigin(identifierOrigin);
    setIdentifierTarget(identifierTarget);
    setDirectional(directional);
}



//##### GETTERS/SETTERS #####


std::string Connection::getIdentifierOrigin()
{
    return this->m_identifierOrigin;
}
std::string Connection::getIdentifierTarget()
{
    return this->m_identifierTarget;
}
bool Connection::getDirectional()
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
void Connection::setDirectional(bool input)
{
    m_directional = input;
}
