#include "NodePoint.h"

void NodePoint::dfsStep(std::unordered_map<std::shared_ptr<Node>,std::shared_ptr<Connection>> unfinishedConnections, std::vector<Connection>& dstConnections)
{
    //I'm a node that represents an intermediate corner
    //I can therefore not complete the construction of any connection
    std::shared_ptr<NodePoint> me(this);

    std::shared_ptr<Connection> con = std::make_shared<Connection>();
    con->setIdentifierOrigin(unfinishedConnections[me]->getIdentifierOrigin());
    con->setIntermediateCorners(unfinishedConnections[me]->getIntermediateCorners());
    con->addIntermediateCorner(std::pair(me->position.x,me->position.y)); 
    
    for(auto& edge : me->edges)
    {
        //push each incident edge's ulterior node onto the stack
        std::shared_ptr<Node>& adjNode = (std::shared_ptr<Node>&)(edge.first == Position::first ? edge.second->getSecondNode().value() : edge.second->getSecondNode());

        unfinishedConnections[adjNode] = con;
        this->dfsStep(unfinishedConnections,dstConnections);
    }
    
}
void NodePoint::connectIncidentEdges(std::vector<std::shared_ptr<Edge>>& edges)
{
//TODO
}