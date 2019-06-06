#include "Shape.h"



//##### GETTERS/SETTERS #####

const std::string Shape::getIdentifier()
{
    return this->m_identifier;  
}
const std::string Shape::getLabel()
{
    return this->m_label;
}
float Shape::getRootCoordX() const
{
	return this->m_rootCoordX;
}
float Shape::getRootCoordY() const
{
	return this->m_rootCoordY;
}

void Shape::setIdentifier(std::string input)
{
    m_identifier = input;
}
void Shape::setLabel(std::string input)
{
    m_label = input;
}
void Shape::setRootCoordX(float input)
{
	m_rootCoordX = input;
}
void Shape::setRootCoordY(float input)
{
	m_rootCoordY = input;
}
