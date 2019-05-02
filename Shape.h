#ifndef SHAPE_H
#define SHAPE_H
class Shape
{
private:
	float rootCoordX;
	float rootCoordY;
protected:

public:
	Shape(/* args */);
	~Shape();
	float getRootCoordX();
	virtual string toTikZ() = 0;
	float getRootCoordX();
	float getRootCoordY();
	void setRootCoordX(float input);
	void setRootCoordY(float input);
};
#endif