#ifndef RORouter_h
#define RORouter_h

#include <string>
#include <functional>
#include "ROEdgeVector.h"
#include "ROEdge.h"

class RONet;
class ROEdgeCont;

class RORouter {
private:
    RONet &_net;
    ROEdgeCont *_source;
public:
    RORouter(RONet &net, ROEdgeCont *source);
    ~RORouter();
    ROEdgeVector compute(ROEdge *from, ROEdge *to);
private:
    class NodeByDistanceComperator : public std::less<ROEdge*> {
    public:
        explicit NodeByDistanceComperator() { }
        ~NodeByDistanceComperator() { }
        bool operator()(ROEdge *nod1, ROEdge *nod2) const {
            return nod1->getEffort()<nod2->getEffort();
        }
    };

    ROEdgeVector dijkstraCompute(ROEdge *from, ROEdge *to);
    ROEdgeVector buildPathFrom(ROEdge *rbegin);
};

#endif
