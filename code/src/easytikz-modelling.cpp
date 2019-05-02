#include "easytikz-modelling.h"
#include <string>

using namespace std;

void Diagram::insertShape(Shape* inShape)
{
    shapes.push_back(inShape);
}

Shape* Diagram::getShape()
{
    throw std::logic_error("not implemented");
}

std::vector<Shape*> Diagram::getShapes()
{
    throw std::logic_error("not implemented");
}

void Diagram::insertConnection(Connection* inConn)
{
    connections.push_back(inConn);
}

Connection* Diagram::getConnection(Connection* inConn)
{
    throw std::logic_error("not implemented");
}

std::vector<Connection*> Diagram::getConnections()
{
    return connections;
}

// Rectangle

