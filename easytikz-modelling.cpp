#include "easytikz-modelling.h"
#include <string>

using namespace std;

Diagram::Diagram(/* args */){}
Diagram::~Diagram(){}
void Diagram::insertShape(Shape* inShape) {
	shapes.insert(shapes.end+1,inShape);
}
Shape* Diagram::getShape(){
    throw std::logic_error("not implemented");
}
vector<Shape*> Diagram::getShapes(){}
void Diagram::insertConnection(Connection* inConn){
    connections.insert(connections.end+1,inConn);
}
Connection* Diagram::getConnection(Connection* inConn){
    throw std::logic_error("not implemented");
}
vector<Connection*> Diagram::getConnections(){
    return connections;
}

// Rectangle

