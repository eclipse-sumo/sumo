#ifndef Distribution_Points_h
#define Distribution_Points_h

#include "Distribution.h"
#include <utils/geom/Position2DVector.h>

class Distribution_Points :
        public Distribution {
public:
    Distribution_Points(const std::string &id,
        const Position2DVector &points);
    virtual ~Distribution_Points();
protected:
    Position2DVector myPoints;
};

#endif
