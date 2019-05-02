#include "Shape.h"

float Shape::getRootCoordX() {
	return 4.5;
}

float Shape::getRootCoordY()
{
	return this->rootCoordY;
}

void Shape::setRootCoordX(float input)
{
	rootCoordX = input;
}
void Shape::setRootCoordY(float input)
{
	rootCoordY = input;
}
