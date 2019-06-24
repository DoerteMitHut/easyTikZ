#include "NodeShape.h"
//OVERRIDES
void NodeShape::dfsStep(std::unordered_map<std::shared_ptr<Node>,std::shared_ptr<Connection>> unfinishedConnections, std::vector<Connection>& dstConnections)
{
    //TODO: make sure branches off of unfinished Connections don't modify the one pointed to. Might be because of the pointers.
        //check whether current node is associated with a path that leads to it
        std::shared_ptr<NodeShape> me(this);
        if(!(unfinishedConnections.find(me) == unfinishedConnections.end()))
        {   //I am a Shape node with an associated incomplete path. I will complete the Connection and add it to dstConnections
            unfinishedConnections[me]->setIdentifierTarget(me->identifier);
            dstConnections.push_back(*unfinishedConnections[me]);
        }

        //If 
        if(!me->markedStart)
        {   //I furthermore declare that I have yet to father a DFS in order to make my offspring known to the world
            //start new DFS as startVertex, king of the andals and the first men
            std::shared_ptr<Connection> con;
            con->setIdentifierOrigin(me->identifier);
            unfinishedConnections[me]=con;
            me->markedStart = true;
            me->markedVisited = true;
            
            for(auto edge : me->edges)
            {
                //push each incident edge's ulterior node onto the stack
                std::shared_ptr<Node>& adjNode = (std::shared_ptr<Node>&)(edge.first == Position::first ? edge.second->getSecondNode().value() : edge.second->getFirstNode()).value();
                if(!adjNode->getMarkedStart())
                {
                    //associate each of them with your new connection and begin dfs on them.
                    unfinishedConnections[adjNode] = con;
                    this->dfsStep(unfinishedConnections,dstConnections);
                }
            }
        }
        else
        {
        std::cout<<"A Shape Node was encoutered twice during a DFS. THIS IS NOT SUPPOSED TO HAPPEN!"<<std::endl;
        } 
}
void NodeShape::connectIncidentEdges(std::vector<std::shared_ptr<Edge>>& edges)
{
    //TODO
}
//SETTER
//ASK: const qualified value parameter. Better for the compiler?
void NodeShape::setShape(Shape p_shape)
{
    shape = p_shape;
}
void NodeShape::setLabel(std::string p_label)
{
    label = p_label;
}
void NodeShape::setIdentifier(std::string p_identifier)
{
    identifier = p_identifier;
}
//GETTER
std::string NodeShape::getLabel() const
{
    return label;
}
std::string NodeShape::getIdentifier() const
{
    return identifier;
}
Shape NodeShape::getShape() const
{
    return shape;
}