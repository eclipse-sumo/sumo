#ifndef ROCompleteRouteDef_h
#define ROCompleteRouteDef_h

#include <string>
#include "ROEdgeVector.h"
#include "RORouteDef.h"

class ROEdge;
class RORoute;
class RORouter;

class ROCompleteRouteDef : public RORouteDef {
protected:
    ROEdgeVector _edges;
public:
    ROCompleteRouteDef(const std::string &id, const ROEdgeVector &edges);
	virtual ~ROCompleteRouteDef();
    ROEdge *getFrom() const;
    ROEdge *getTo() const;
protected:
    RORoute *buildCurrentRoute(RORouter &router, long begin);
    void addAlternative(RORoute *current, long begin);
    void xmlOutCurrent(std::ostream &res) const;
    void xmlOutAlternatives(std::ostream &altres) const;
//    RORoute *buildRoute(DijkstraRouter &router) const;
};

#endif
