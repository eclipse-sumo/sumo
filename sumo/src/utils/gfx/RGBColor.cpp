#include "RGBColor.h"

RGBColor::RGBColor(double red, double green, double blue)
    : _red(red), _green(green), _blue(blue)
{
}


RGBColor::~RGBColor()
{
}


double 
RGBColor::red() const
{
    return _red;
}


double 
RGBColor::green() const
{
    return _green;
}


double 
RGBColor::blue() const
{
    return _blue;
}


