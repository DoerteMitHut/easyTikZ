#pragma once
#include "Node.h"
#include "Edge.h"

class NodePoint : public Node
{
    public:
        NodePoint(){};
        NodePoint(cv::Point2d p_position)
        {
            position = p_position;
        };
        
        void dfsStep(std::unordered_map<std::shared_ptr<Node>,std::shared_ptr<Connection>> unfinishedConnections, std::vector<Connection>& dstConnections) override;

        void connectIncidentEdges(std::vector<std::shared_ptr<Edge>>& edges) override;
};