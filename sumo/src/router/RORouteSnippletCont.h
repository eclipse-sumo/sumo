#ifndef RORouteSnippletCont_h
#define RORouteSnippletCont_h

#include <map>
#include "ROEdgeVector.h"

class ROEdge;

class RORouteSnippletCont {
private:
    typedef std::map<std::pair<ROEdge*, ROEdge*>, ROEdgeVector > MapType;
    MapType _known;
public:
    RORouteSnippletCont();
    ~RORouteSnippletCont();
    bool add(const ROEdgeVector &item);
    const ROEdgeVector &get(ROEdge *from, ROEdge *to) const;
    bool knows(ROEdge *from, ROEdge *to) const;
private:
    /// we made the copy constructor invalid
    RORouteSnippletCont(const RORouteSnippletCont &src);
    /// we made the assignment operator invalid
    RORouteSnippletCont &operator=(const RORouteSnippletCont &src);
};

#endif
