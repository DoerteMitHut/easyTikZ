#include "Node.h"

void Node::setPosition(cv::Point2d pos)
{
    position = pos;
}
void Node::setIncidentEdges(std::vector<std::pair<Position,std::shared_ptr<Edge>>> incidentEdges)
{
    edges = incidentEdges;
}
void Node::setMarkedStart(bool val)
{
    markedStart = val;
}
void Node::setMarkedVisited(bool val)
{
    markedVisited = val;
}
//GETTER
cv::Point2d Node::getPosition() const {
    return position;
}
std::vector<std::pair<Position,std::shared_ptr<Edge>>> Node::getIncidentEdges()const
{
    return edges;
}
bool Node::getMarkedStart() const
{
    return markedStart;
}
bool Node::getMarkedVisited() const
{
    return markedVisited;
}


