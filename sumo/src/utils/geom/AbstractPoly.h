#ifndef AbstractPoly_h
#define AbstractPoly_h

#include "Position2D.h"

class AbstractPoly {
public:
    AbstractPoly() { }
    virtual ~AbstractPoly() { }
    virtual bool around(const Position2D &p, double offset=0) const = 0;
    virtual bool overlapsWith(const AbstractPoly &poly, double offset=0) const = 0;
    virtual bool partialWithin(const AbstractPoly &poly, double offset=0) const = 0;
    virtual bool crosses(const Position2D &p1,
        const Position2D &p2) const = 0;
};

#endif
