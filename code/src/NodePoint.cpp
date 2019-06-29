#include "NodePoint.h"

void NodePoint::dfsStep(std::unordered_map<std::shared_ptr<Node>,std::shared_ptr<Connection>>& unfinishedConnections, std::vector<Connection>& dstConnections)
{
    //I'm a node that represents an intermediate corner
    //I can therefore not complete the construction of any connection

    std::shared_ptr<Connection> con = std::make_shared<Connection>();
    con->setIdentifierOrigin(unfinishedConnections[shared_from_this()]->getIdentifierOrigin());
    con->setIntermediateCorners(unfinishedConnections[shared_from_this()]->getIntermediateCorners());
    con->addIntermediateCorner(std::pair(position.x,position.y)); 
    
    markedStart = true;
    markedVisited = true;

    for(auto& edge : edges)
    {
        //push each incident edge's ulterior node onto the stack
        std::shared_ptr<Node> adjNode = (edge.first == Position::first ?
        edge.second->getSecondNode().value() :
        edge.second->getFirstNode()).value() ;

        if(!adjNode->getMarkedStart() && adjNode)
        {
            unfinishedConnections[adjNode] = con;
            adjNode->dfsStep(unfinishedConnections,dstConnections);
        }
    }
    
}
void NodePoint::connectIncidentEdges(std::vector<std::shared_ptr<Edge>>& edges)
{
//TODO
}