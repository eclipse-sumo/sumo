#include <utility>
#include "Boundery.h"

Boundery::Boundery()
    : _xmin(10000000000.0), _xmax(-10000000000.0), 
    _ymin(10000000000.0), _ymax(-10000000000.0)
{
}

Boundery::Boundery(double x1, double y1, double x2, double y2)
    : _xmin(x1), _xmax(x1), _ymin(y1), _ymax(y1)
{
    add(x2, y2);
}


Boundery::~Boundery()
{
}


void
Boundery::add(double x, double y)
{
    _xmin = _xmin < x ? _xmin : x;
    _xmax = _xmax > x ? _xmax : x;
    _ymin = _ymin < y ? _ymin : y;
    _ymax = _ymax > y ? _ymax : y;
} 


std::pair<double, double>
Boundery::getCenter() const
{
    return std::pair<double, double>
        ( (_xmin+_xmax)/2.0, (_ymin+_ymax)/2.0);
}

double
Boundery::xmin() const
{
    return _xmin;
}


double
Boundery::xmax() const
{
    return _xmax;
}


double
Boundery::ymin() const
{
    return _ymin;
}


double
Boundery::ymax() const
{
    return _ymax;
}


double
Boundery::getWidth() const
{
    return _xmax - _xmin;
}


double
Boundery::getHeight() const
{
    return _ymax - _ymin;
}


