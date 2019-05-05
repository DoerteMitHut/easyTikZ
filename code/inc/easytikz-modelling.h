#ifndef EASYTIKZ_MODELLING_H
#define EASYTIKZ_MODELLING_H

#include "Shape.h"
#include "Connection.h"
#include <vector>
#include <stdexcept>

//using namespace cv;

class Diagram
{
public:
    Diagram(/* args */) = default;
    ~Diagram() = default;
    void insertShape(Shape*);
    void insertConnection(Connection*);
    Shape* getShape();
    std::vector<Shape*> getShapes();
    Connection* getConnection(Connection*);
    std::vector<Connection*> getConnections();

private:
    std::vector<Shape*> shapes;
    std::vector<Connection*> connections;
};

//=======================================================
//==== Shape derivatives ================================
//=======================================================
//=======================================================
#endif
