#include <string>
#include "ROEdgeVector.h"
#include "RONode.h"

RONode::RONode(const std::string &id, const ROEdgeVector &edges)
    : _edges(edges)
{
}

RONode::~RONode()
{
}

