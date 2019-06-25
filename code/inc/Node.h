#pragma once
#include <opencv2/opencv.hpp>
#include <memory>
#include "Connection.h"

class Edge;

enum Position{
    first,
    second
};

class Node 
{
     ///////// MEMBER VARIABLES ////////////////////
    ///////////////////////////////////////////////
    protected:
        bool markedVisited;
        bool markedStart = false;
        cv::Point2d position;
        std::vector<std::pair<Position,std::shared_ptr<Edge>>> edges;

     ///////// MEMBER FUNCTIONS ////////////////////
    ///////////////////////////////////////////////

    public:
        //VIRTUAL
        virtual void dfsStep(std::unordered_map<std::shared_ptr<Node>,std::shared_ptr<Connection>> unfinishedConnections, std::vector<Connection>& dstConnections) = 0;
        virtual void connectIncidentEdges(std::vector<std::shared_ptr<Edge>>& edges) = 0;
        //SETTER
        void setPosition(cv::Point2d pos);
        void setIncidentEdges(std::vector<std::pair<Position,std::shared_ptr<Edge>>> incidentEdges);
        void setMarkedStart(bool val);
        void setMarkedVisited(bool val);
        //GETTER
        cv::Point2d getPosition() const;
        std::vector<std::pair<Position,std::shared_ptr<Edge>>> getIncidentEdges() const;
        bool getMarkedStart() const;
        bool getMarkedVisited() const;
};
