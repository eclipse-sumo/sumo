#ifndef RONode_h
#define RONode_h

#include <string>
#include "ROEdgeVector.h"

class RONode {
private:
    ROEdgeVector _edges;
public:
    RONode(const std::string &id, const ROEdgeVector &edges);
    ~RONode();
}; 

#endif
