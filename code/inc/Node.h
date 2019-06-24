#pragma once

class Node 
{
    //Not sure if a standard constructor is needed
    Node(){}
    //constructor for shapeless node
    Node(cv::Point2d p_position):position(p_position){}
    //constructor for shape node
    Node(cv::Point2d p_position, std::optional<Shape> p_shape)
         :position(p_position),shape(p_shape)
    {}
    Node(cv::Point2d p_position):position(p_position){}
    bool markedVisited;
    bool markedStart = false;
    cv::Point2d position;
    std::optional<Shape> shape;
    std::vector<std::pair<Position,std::shared_ptr<Edge>>> edges;
};
template <typename T>
struct MyShape
{
    std::string identifier;
    T shape;
};

struct Edge;
struct PointNode : Node
{
    //TODO implement different structs for Nodes
};

struct ShapeNode : Node
{
    //TODO implement different structs for Nodes
};

struct Edge
{   
    Edge(cv::Vec4d p_line, std::pair<std::shared_ptr<Node>,std::shared_ptr<Node>> p_nodes):line(p_line),nodes(p_nodes)
    {}
    cv::Vec4d line;
    std::pair<std::optional<std::shared_ptr<Node>>,std::optional<std::shared_ptr<Node>>> nodes;
    bool markedVisited = false;
};