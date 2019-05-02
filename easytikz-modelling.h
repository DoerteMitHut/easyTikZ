#ifndef EASYTIKZ_MODELLING_H
#define EASYTIKZ_MODELLING_H

#include <stdio.h>
#include "Shape.h"
#include "Connection.h"
#include <vector>
#include <stdexcept>

using namespace std;
//using namespace cv;

class Diagram
{
private:
	vector<Shape*> shapes;
	vector<Connection*> connections;
public:
	Diagram(/* args */) {}
	~Diagram() {}
	void insertShape(Shape*); 
	void insertConnection(Connection*);
	Shape* getShape();
	vector<Shape*> getShapes();
	Connection* getConnection(Connection*);
	vector<Connection*> getConnections();
};

Diagram::Diagram(/* args */)
{
}

Diagram::~Diagram()
{
}

//=======================================================
//==== Shape derivatives ================================
//=======================================================
//=======================================================
#endif