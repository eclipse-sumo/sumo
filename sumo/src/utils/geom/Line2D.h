#ifndef Line2D_h
#define Line2D_h

#include "Position2D.h"
#include <utils/common/DoubleVector.h>

class Position2DVector;

class Line2D {
public:
    Line2D();
    Line2D(const Position2D &p1, const Position2D &p2);
    ~Line2D();
    void extrapolateBy(double length);
    void extrapolateFirstBy(double length);
    void extrapolateSecondBy(double length);
    const Position2D &p1() const;
    const Position2D &p2() const;
    Position2D getPositionAtDistance(double offset) const;
    void move2side(double amount);
    DoubleVector intersectsAtLengths(const Position2DVector &v);
    double atan2Angle() const;
    double atan2DegreeAngle() const;
    double atan2PositiveAngle() const;
    bool intersects(const Line2D &l) const;
    Position2D intersectsAt(const Line2D &l) const;
    double length() const;
    void add(double x, double y);
    void sub(double x, double y);
    double distanceTo(const Position2D &p) const;
    Line2D &reverse();
    double nearestPositionTo(const Position2D &p);

private:
    Position2D myP1, myP2;
};

#endif

