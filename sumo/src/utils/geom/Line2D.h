#ifndef Line2D_h
#define Line2D_h

#include "Position2D.h"

class Line2D {
public:
    Line2D();
    Line2D(const Position2D &p1, const Position2D &p2);
    ~Line2D();
    void extrapolateBy(double length);
    const Position2D &p1() const;
    const Position2D &p2() const;
    Position2D getPositionAtDistance(double offset) const;
    void move2side(double amount);
private:
    Position2D myP1, myP2;
};

#endif

