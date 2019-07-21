#include "NodePoint.h"

void NodePoint::dfsStep(std::unordered_map<std::shared_ptr<Node>,std::shared_ptr<Connection>>& unfinishedConnections, std::vector<Connection>& dstConnections)
{
    std::cout<<"$ - DFS called on Point Node at "<<position.x<<"|"<<position.y<<std::endl;
    //I'm a node that represents an intermediate corner
    //I can therefore not complete the construction of any connection

    std::shared_ptr<Connection> con = std::make_shared<Connection>();
    con->setIdentifierOrigin(unfinishedConnections[shared_from_this()]->getIdentifierOrigin());
    con->setIntermediateCorners(unfinishedConnections[shared_from_this()]->getIntermediateCorners());
    con->addIntermediateCorner(std::pair(position.x/100,position.y/-100)); 
    std::cout<<"$ - added myself"<<std::endl;
    markedStart = true;
    markedVisited = true;
    std::cout<<"$ - marked myself"<<std::endl;
    for(auto& edge : edges)
    {
        //push each incident edge's ulterior node onto the stack
        std::optional<std::shared_ptr<Node>> adjNodeOpt = (edge.first == Position::first ?
        edge.second->getSecondNode() :
        edge.second->getFirstNode()) ;
        std::shared_ptr<Node> adjNode;
        if(adjNodeOpt)
        {
            adjNode = adjNodeOpt.value();

            if(!adjNode->getMarkedStart())
            {
                unfinishedConnections[adjNode] = con;
                adjNode->dfsStep(unfinishedConnections,dstConnections);
            }
            else
            {
                std::cout<<"$ - adjacent node marked"<<std::endl;
            }
        }
        else
        {
            std::cout<<"$ - adjacent node empty"<<std::endl;
        }
    }
    
}