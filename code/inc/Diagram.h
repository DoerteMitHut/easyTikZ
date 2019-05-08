#ifndef EASYTIKZ_MODELLING_H
#define EASYTIKZ_MODELLING_H

#include "Shapes.h"
#include "Connection.h"
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <typeinfo>

//using namespace cv;

class Diagram
{
public:
    Diagram(/* args */) = default;
    ~Diagram() = default;
    
    //fill diagram
    void insertNode(Shape&);
    void insertConnection(Connection&);
    
    
    Shape& getShape();
    
    //QUESTION:bad style to have overloads with same name serve different purposes?
    std::unordered_map<std::type_index,std::vector<std::unique_ptr<Shape>>> getShapes();
    std::vector<std::unique_ptr<Shape>>& getShapes(std::type_index);
    std::vector<Connection>& getConnections();

private:
    //member variables
    std::unordered_map<std::type_index,std::vector<std::unique_ptr<Shape>>> m_nodes;
    std::vector<Connection> m_connections;

    double m_gridSize;
};

//=======================================================
//==== Shape derivatives ================================
//=======================================================
//=======================================================
#endif
