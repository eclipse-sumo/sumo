#include "Distribution.h"
#include <utils/geom/Position2DVector.h>
#include "Distribution_Points.h"

Distribution_Points::Distribution_Points(const std::string &id,
                                         const Position2DVector &points)
    : Distribution(id), myPoints(points)
{
}


Distribution_Points::~Distribution_Points()
{
}
