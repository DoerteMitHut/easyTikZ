#include "easytikz-modelling.h"
#include <string>

using namespace std;

float Shape::getRootCoordX(){
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

// Rectangle

float Rectangle::getSize()
{
	return size;
}
int Rectangle::getVerts()
{
	return verts;
}

void Rectangle::setSize(float input)
{
	size = input;
}
void Rectangle::setVerts(int input)
{
	verts = input;
}

Rectangle::Rectangle()
{
	setRootCoordX(0);
	setRootCoordY(0);
}
Rectangle::Rectangle(float rootCoordX, float rootCoordY)
{
	setRootCoordX(rootCoordX);
	setRootCoordY(rootCoordY);
}