#include "ROLane.h"

ROLane::ROLane(double length, double maxSpeed)
    : _length(length), _maxSpeed(maxSpeed)
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
