#pragma once

#include "Shapes.h"
#include "Connection.h"

#include <memory>
#include <string>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>

//using namespace cv;


class Diagram
{
public:
    Diagram(/* args */) = default;
    ~Diagram() = default;
    
    //fill diagram
    void insertRectangle(Rectangle&);
    void insertConnection(Connection&);
    
    Shape& getShape();
    
    std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>> getShapes();
    std::vector<std::shared_ptr<Shape>>& getShapes(std::type_index);
    std::vector<Connection>& getConnections();

private:
    std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>> m_nodes;
    std::vector<Connection> m_connections;
    double m_gridSize;
};

