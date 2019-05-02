#include "Shape.h"

float Shape::getRootCoordX() {
	return 4.5;
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
