#pragma once
#include "Node.h"
#include "Edge.h"
#include "Shape.h"

class NodeShape : public Node
{   
     

    private:
        Shape shape;
        std::string identifier;
        std::string label = "";
        double innerRad;
        double outerRad;

    public:
        NodeShape(){};
        NodeShape(cv::Point2d p_position,Shape p_shape,std::string p_identifier):shape(p_shape),identifier(p_identifier)
        {
            position = p_position;
        };
        //OVERRIDES
        void dfsStep(std::unordered_map<std::shared_ptr<Node>,std::shared_ptr<Connection>>& unfinishedConnections, std::vector<Connection>& dstConnections) override;
        void connectIncidentEdges(std::vector<std::shared_ptr<Edge>>& edges);
        //SETTER
        //ASK: const qualified value parameter. Better for the compiler?
        void setShape(Shape p_shape);
        void setLabel(std::string p_label);
        void setIdentifier(std::string p_identifier);
        void setInnerRad(double rad);
        void setOuterRad(double rad);
        //GETTER
        std::string getLabel() const;
        std::string getIdentifier() const;
        Shape getShape() const;
        double getInnerRad() const;
        double getOuterRad() const;
};