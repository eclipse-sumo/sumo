#ifndef RORoute_h
#define RORoute_h

#include <string>
#include <utils/common/Named.h>
#include "ROEdgeVector.h"

class ROEdge;

class RORoute : public Named {
private:
    double _costs;
    double _propability;
//    size_t _references;
	ROEdgeVector _route;
public:
    RORoute(const std::string &id, double costs, double prop,
        const ROEdgeVector &route);
	~RORoute();
    void add(ROEdge *id);
    void xmlOut(std::ostream &os) const;
    void xmlOutEdges(std::ostream &os) const;
    ROEdge *getFirst() const;
    ROEdge *getLast() const;
    double getCosts() const;
    double getPropability() const;
    void setCosts(double costs);
    void setPropability(double prop);
    double recomputeCosts(long begin) const;
    bool equals(RORoute *ro) const;
    size_t size() const;
};

#endif
