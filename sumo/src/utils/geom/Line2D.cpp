#include "Position2D.h"
#include "Line2D.h"
#include "GeomHelper.h"
#include <cassert>

Line2D::Line2D()
{
}


Line2D::Line2D(const Position2D &p1, const Position2D &p2)
    : myP1(p1), myP2(p2)
{
}


Line2D::~Line2D()
{
}


void
Line2D::extrapolateBy(double length)
{
    double oldlen = GeomHelper::distance(myP1, myP2);
    double x1 = myP1.x() - (myP2.x() - myP1.x()) * (length) / oldlen;
    double y1 = myP1.y() - (myP2.y() - myP1.y()) * (length) / oldlen;
    double x2 = myP2.x() - (myP1.x() - myP2.x()) * (length) / oldlen;
    double y2 = myP2.y() - (myP1.y() - myP2.y()) * (length) / oldlen;
    myP1 = Position2D(x1, y1);
    myP2 = Position2D(x2, y2);
}


const Position2D &
Line2D::p1() const
{
    return myP1;
}


const Position2D &
Line2D::p2() const
{
    return myP2;
}


Position2D
Line2D::getPositionAtDistance(double offset) const
{
    double length = GeomHelper::distance(myP1, myP2);
    double x = myP1.x() + (myP2.x() - myP1.x()) / length * offset;
    double y = myP1.y() + (myP2.y() - myP1.y()) / length * offset;
/*    double x2 = myP2.x() - (myP1.x() - myP2.x())  / length * offset;
    double y2 = myP2.y() - (myP1.y() - myP2.y()) / length * offset;*/
    return Position2D(x, y);
}


void
Line2D::move2side(double amount)
{
    std::pair<double, double> p = GeomHelper::getNormal90D_CW(myP1, myP2, amount);
    myP1.add(p.first, p.second);
    myP2.add(p.first, p.second);
}


DoubleVector
Line2D::intersectsAtLengths(const Position2DVector &v)
{
    assert(v.intersects(myP1, myP2));
    Position2DVector p = v.intersectsAtPoints(myP1, myP2);
    DoubleVector ret;
    for(size_t i=0; i<p.size(); i++) {
        ret.push_back(GeomHelper::distance(myP1, p.at(i)));
    }
    return ret;
}


double
Line2D::atan2Angle() const
{
    return atan2(myP1.x()-myP2.x(), myP1.y()-myP2.y());
}


double
Line2D::atan2DegreeAngle() const
{
    return atan2(myP1.x()-myP2.x(), myP1.y()-myP2.y()) * 180.0 / 3.1415926535897932384626433832795;
}


Position2D
Line2D::intersectsAt(const Line2D &l) const
{
    return GeomHelper::intersection_position(myP1, myP2, l.myP1, l.myP2);
}
