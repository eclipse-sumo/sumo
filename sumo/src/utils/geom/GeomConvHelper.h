#ifndef GeomConvHelper_h
#define GeomConvHelper_h

#include <string>
#include <utils/geom/Position2DVector.h>

class GeomConvHelper {
public:
    static Position2DVector parseShape(const std::string &shpdef);

};


#endif
