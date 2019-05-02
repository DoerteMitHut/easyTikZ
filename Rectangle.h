#ifndef RECTANGLE_H
#define RECTANGLE_H
class Rectangle :Shape
{
private:

protected:
	int verts;
	float size;
	//group of float[2][1] arrays for connection hardpoints?

public:
	float getSize();
	int getVerts();
	void setSize(float input);
	void setVerts(int input);
	Rectangle();

	Rectangle(float rootCoordX, float rootCoordY);
};
#endif