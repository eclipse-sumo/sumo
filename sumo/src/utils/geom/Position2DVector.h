#ifndef Position2DVector_h
#define Position2DVector_h

#include <vector>
#include "AbstractPoly.h"
#include "Boundery.h"
#include "Position2D.h"

class Position2DVector
        : public AbstractPoly {
public:
    Position2DVector();
    ~Position2DVector();
    void push_back(const Position2D &p);
    bool around(const Position2D &p, double offset=0) const;
    bool overlapsWith(const AbstractPoly &poly, double offset=0) const;
    bool intersects(const Position2D &p1, const Position2D &p2) const;
    bool intersects(const Position2DVector &v1) const;
    Position2D intersectsAtPoint(const Position2D &p1,
        const Position2D &p2) const;
    Position2D intersectsAtPoint(const Position2DVector &v1) const;
    void clear();
    const Position2D &at(size_t i) const;
    size_t size() const;
    Position2D positionAtLengthPosition(double pos) const;
    static Position2D positionAtLengthPosition(const Position2D &p1,
        const Position2D &p2, double pos);
    Boundery getBoxBoundery() const;
    Position2D center() const;
    double length() const;
    bool partialWithin(const AbstractPoly &poly, double offset=0) const;
    bool crosses(const Position2D &p1, const Position2D &p2) const;
    const Position2D &getBegin() const;
    const Position2D &getEnd() const;

private:
    typedef std::vector<Position2D> ContType;
    ContType myCont;
};

#endif
