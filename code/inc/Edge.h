#pragma once
#include "Node.h"

//Cant use abstract class Node for this
typedef std::pair<std::optional<std::shared_ptr<Node>>,std::optional<std::shared_ptr<Node>>> NodePair;

class Edge
{  

     ///////// CONSTRUCTORS ////////////////////////
    ///////////////////////////////////////////////
    
     ///////// MEMBER VARIABLES ////////////////////
    ///////////////////////////////////////////////
    private:
        cv::Vec4d line;
        NodePair nodes;
     ///////// MEMBER FUNCTIONS ////////////////////
    ///////////////////////////////////////////////
    public:
        //CONSTRUCTORS
        Edge(){};
        Edge(cv::Vec4d p_line):line(p_line){}; 
        Edge(cv::Vec4d p_line, NodePair p_nodes):line(p_line), nodes(p_nodes){};
        //SETTER        
        void setLine(cv::Vec4d p_line);
        void setFirstNode(std::shared_ptr<Node> p_node);
        void setSecondNode(std::shared_ptr<Node> p_node);
        void setNodes(NodePair p_nodes);
        //GETTER
        cv::Vec4d getLine() const;
        std::optional<std::shared_ptr<Node>> getFirstNode() const;
        std::optional<std::shared_ptr<Node>> getSecondNode() const;
        NodePair getNodes() const;

};