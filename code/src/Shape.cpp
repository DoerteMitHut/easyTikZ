#include "Shape.h"

float Shape::getRootCoordX()
{
	return this->m_rootCoordX;
}

float Shape::getRootCoordY()
{
	return this->m_rootCoordY;
}

void Shape::setRootCoordX(float input)
{
	m_rootCoordX = input;
}
void Shape::setRootCoordY(float input)
{
	m_rootCoordY = input;
}
