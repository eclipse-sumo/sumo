#include <map>
#include <proj_api.h>
#include "GeoConvHelper.h"

projPJ GeoConvHelper::myProjection = 0;
Position2D GeoConvHelper::myOffset;

void
GeoConvHelper::init(const std::string &proj, const Position2D &offset)
{
    delete myProjection;
    myProjection = pj_init_plus("+proj=utm +zone=33 +ellps=bessel +units=m");
    myOffset = offset;
}


bool
GeoConvHelper::initialised()
{
    return myProjection!=0;
}


void
GeoConvHelper::close()
{
    delete myProjection;
    myProjection = 0;
}


void
GeoConvHelper::cartesian2geo(Position2D &cartesian)
{
    projUV p;
    p.u = cartesian.x() - myOffset.x();
    p.v = cartesian.y() - myOffset.y();
    p = pj_inv(p, myProjection);
    p.u *= RAD_TO_DEG;
    p.v *= RAD_TO_DEG;
    cartesian.set((SUMOReal) p.u, (SUMOReal) p.v);
}


