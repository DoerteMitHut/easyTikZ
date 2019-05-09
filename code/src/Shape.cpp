#include "Shape.h"



//##### GETTERS/SETTERS #####

std::string Shape::getIdentifier()
{
    return this->m_identifier;  
}
float Shape::getRootCoordX()
{
	return this->m_rootCoordX;
}
float Shape::getRootCoordY()
{
	return this->m_rootCoordY;
}

void Shape::setIdentifier(std::string input)
{
    m_identifier = input;
}
void Shape::setRootCoordX(float input)
{
	m_rootCoordX = input;
}
void Shape::setRootCoordY(float input)
{
	m_rootCoordY = input;
}
