#ifndef ConvHelper_h
#define ConvHelper_h

#include <string>
#include <utils/geom/Position2DVector.h>

class ConvHelper {
public:
    static Position2DVector parseShape(const std::string &shpdef);
};


#endif
