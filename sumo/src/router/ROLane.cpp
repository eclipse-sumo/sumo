#include <utils/common/Named.h>
#include "ROLane.h"

ROLane::ROLane(const std::string &id, double length, double maxSpeed)
    : Named(id), _length(length), _maxSpeed(maxSpeed)
{
}


ROLane::~ROLane()
{
}

/*
double
ROLane::getWeight() const {
    return _length / _maxSpeed;
}
*/

double
ROLane::getLength() const
{
    return _length;
}
