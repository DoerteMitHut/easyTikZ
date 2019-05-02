#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Shape.h"

class Rectangle : public Shape
{
public:
    Rectangle();
    Rectangle(float rootCoordX, float rootCoordY);

    float getSize();
    int getVerts();
    void setSize(float input);
    void setVerts(int input);

protected:
	int verts;
	float size;
	//group of float[2][1] arrays for connection hardpoints?
};
#endif
