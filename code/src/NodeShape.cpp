#include "NodeShape.h"
#include "utils.h"
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
                std::shared_ptr<Node> adjNode = (std::shared_ptr<Node>)(edge.first == Position::first ? edge.second->getSecondNode().value() :
                edge.second->getFirstNode()).value();
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
void NodeShape::connectIncidentEdges(std::vector<std::shared_ptr<Edge>>& inEdges)
{
    
    std::cout.setstate(std::ios_base::failbit);
    
    //iterate over all given Edges
    for(std::shared_ptr<Edge>& e : inEdges)
    {
        //extract line segment from given Edge
        cv::Vec4d line = e->getLine();
        
        //store Endpoints of LineSegment
        cv::Point2d endPointL(line[0],line[1]);
        cv::Point2d endPointR(line[2],line[3]);

        //vector connecting the two endpoints
        cv::Point2d vec = endPointR-endPointL; 

        //vector from left endpoint to shape centroid
        cv::Point2d centroidVec = position-endPointL;

        //check whether left Endpoint lies within the outer radius of the shape
        if(twoPointDist(endPointL,position)<=outerRad)
        {   
            //normalize line vector so that taking the dot product of a vector a with it is equivalent to projecting it onto vec.
            double normVec;
            normVec = pointNorm(vec);
            vec /= normVec;
            
            //orthogonally project the centroid onto the line vector
            cv::Point2d projPoint = vec * pointDotProduct(vec,centroidVec);
            //get vector between centroid and projection foot
            cv::Point2d projvec = projPoint-centroidVec;
            
            //if line through line segment passes through an innerRad-circle around the shape centroid, shape and line are considered incident 
            if((pointNorm(projvec) < innerRad))
            {
                //edge is added to the nodes list of incident Edges
                edges.push_back(std::pair(Position::first,e));
                //Node is set as first node of the edge
                e->setFirstNode(std::make_shared<NodeShape>(this));
                continue;
            }
        }
        else{
            // Left endpoint is not inside outer radius
        }
        if(twoPointDist(endPointR,position)<=outerRad)
        {
            vec = endPointL-endPointR; 
            centroidVec = position-endPointR;

            //normalize line vector so that taking the dot product of a vector a with it is equivalent to projecting it onto vec.
            double normVec;
            normVec = pointNorm(vec);
            vec /= normVec;
            
            //orthogonally project the centroid onto the line vector
            cv::Point2d projPoint = vec * pointDotProduct(vec,centroidVec);
            //get vector between centroid and projection foot
            cv::Point2d projvec = projPoint-centroidVec;
            
            //if line through line segment passes through an innerRad-circle around the shape centroid, shape and line are considered incident 
            if((pointNorm(projvec) < innerRad))
            {
                //edge is added to the nodes list of incident Edges
                edges.push_back(std::pair(Position::second,e));
                //Node is set as second node of the edge
                e->setSecondNode(std::make_shared<NodeShape>(this));
                continue;
            }
        }
        else{
            // Left endpoint is not inside outer radius
        }
    }
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
void NodeShape::setInnerRad(double rad)
{
    innerRad = rad;
}
void NodeShape::setOuterRad(double rad)
{
    outerRad = rad;
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
double NodeShape::getInnerRad() const
{
    return innerRad;
}
double NodeShape::getOuterRad() const
{
    return outerRad;
}