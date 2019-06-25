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
void NodeShape::connectIncidentEdges(std::vector<std::shared_ptr<Edge>>& edges)
{
    //TODO complete function
    std::cout.setstate(std::ios_base::failbit);
    cv::Point2d centroid(shape.setRootCoordX(),shape.getRootCoordY());
    
    // for(const cv::Point2d &  pt : shape)
    // {
    //     centroid.x += pt.x;
    //     centroid.y += pt.y;
    // }
    // centroid.x /= shape.size();
    // centroid.y /= shape.size();

    double outterRad = 0;
    for (const cv::Point2d& pt : shape)
    {
        if (twoPointDist(pt,centroid) > outterRad)
        {
            outterRad = twoPointDist(pt,centroid);
        }
    }
    outterRad *=1.5;
    double innerRad = outterRad; 

    for(int i = 1; i< shape.size();i++)
    {
        cv::Point2d p = shape[i-1];
        cv::Point2d q = shape[i%shape.size()];

        cv::Point2d vec = cv::Point2d((q.x-p.x)/2.,(q.y-p.y)/2.);
        cv::Point2d center = p+vec;

        if(twoPointDist(center,centroid)<innerRad)
        {
            innerRad = twoPointDist(center,centroid);
        }
    }

    for(std::shared_ptr<Edge> e : edges)
    {
        cv::Vec4d line = e->getLine();
        std::cout<<"=================================\n=================================\n"<<"Testing Line: "<<"("<<line[0]<<"|"<<line[1]<<")--("<<line[2]<<"|"<<line[3]<<")\nwith shape centroid "<<centroid.x<<"|"<<centroid.y<<std::endl;
        cv::Point2d endPointL(line[0],line[1]);
        cv::Point2d endPointR(line[2],line[3]);
        cv::Point2d vec = endPointR-endPointL; 
        std::cout<<"vec is "<<"("<<vec.x<<"|"<<vec.y<<")"<<std::endl;
        cv::Point2d centroidVec = centroid-endPointL;
        std::cout<<"centroidVec is "<<"("<<centroidVec.x<<"|"<<centroidVec.y<<")"<<std::endl;
        if(twoPointDist(endPointL,centroid)<=outterRad)
        {   
            std::cout<<"====================\n"<<"left point inside outer radius"<<std::endl;
            double normVec;
            normVec = pointNorm(vec);
            std::cout << "length of line is "<<normVec<<std::endl;
            vec /= normVec;
            std::cout << "normalized vector is "<<vec.x<<"|"<<vec.y<<std::endl;
            cv::Point2d projPoint = vec * pointDotProduct(vec,centroidVec);
            std::cout <<"projected point is "<<projPoint.x<<"|"<<projPoint.y<<std::endl;
            cv::Point2d projvec = projPoint-centroidVec;
            std::cout<<"projection vector is "<<projvec.x<<"|"<<projvec.y<<std::endl;
            if((pointNorm(projvec) < innerRad))
            {
                std::cout << "length "<<pointNorm(projvec)<< " of projVec is smaller than inner radius"<<std::endl;
                dstEdges.push_back(std::pair(Position::first,e));
                continue;
            }
        }
        else{
            std::cout<< "left point not inside outer radius"<<std::endl;
        }
        if(twoPointDist(endPointR,centroid)<=outterRad)
        {
            std::cout<<"====================\n"<<"right point inside outter radius"<<std::endl;
            vec = endPointL-endPointR; 
            std::cout<<"vec is "<<"("<<vec.x<<"|"<<vec.y<<")"<<std::endl;
            centroidVec = centroid-endPointR;
            std::cout<<"centroidVec is "<<"("<<centroidVec.x<<"|"<<centroidVec.y<<")"<<std::endl;

            double normVec;
            normVec = pointNorm(vec);
            std::cout << "length of line is "<<normVec<<std::endl;
            vec /= normVec;
            std::cout << "normalized vector is "<<vec.x<<"|"<<vec.y<<std::endl;
            double dottiProducti = pointDotProduct(vec,centroidVec);
            std::cout << "dottiproducti is "<< dottiProducti<<std::endl;
            
            cv::Point2d projPoint = vec * dottiProducti;
            std::cout <<"projected point is "<<projPoint.x<<"|"<<projPoint.y<<std::endl;
            cv::Point2d projvec = projPoint-centroidVec;
            std::cout<<"projection vector is "<<projvec.x<<"|"<<projvec.y<<std::endl;
            if((pointNorm(projvec) < innerRad))
            {
                std::cout << "length "<<pointNorm(projvec)<< " of projVec is smaller than inner radius"<<std::endl;
                dstEdges.push_back(std::pair(Position::second,e));
                continue;
            }
        }
        else{
            std::cout<< "right point not inside outer radius"<<std::endl;
        }
    }
    std::cout<< "shape has "<< dstEdges.size() << " incident lines"<<std::endl;
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