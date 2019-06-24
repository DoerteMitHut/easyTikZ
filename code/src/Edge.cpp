#include "Edge.h"

void Edge::setLine(cv::Vec4d p_line)
{
    line = p_line;
}
void Edge::setFirstNode(std::shared_ptr<Node> p_node)
{
    nodes.first = p_node;
}
void Edge::setSecondNode(std::shared_ptr<Node> p_node)
{
    nodes.second = p_node;
}
void Edge::setNodes(NodePair p_nodes)
{
    nodes = p_nodes;
}
//Edge::getTER
cv::Vec4d Edge::getLine() const
{
    return line;
}
std::optional<std::shared_ptr<Node>> Edge::getFirstNode() const
{
    return nodes.first;
}
std::optional<std::shared_ptr<Node>> Edge::getSecondNode() const
{
    return nodes.second;
}
NodePair Edge::getNodes() const
{
    return nodes;
}
