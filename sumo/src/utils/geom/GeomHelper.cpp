#include "GeomHelper.h"

bool GeomHelper::intersects(double x1b, double y1b, double x1e, double y1e,
                            double x2b, double y2b, double x2e, double y2e)
{
    double b1 = (y1e-y1b)/(x1e-x1b);
    double b2 = (y2e-y2b)/(x1e-x2b);
    double a1 = y1b-b1*x1b;
    double a2 = y2b-b2*x2b;
    double xi = -(a1-a2)/(b1-b2);
    double yi = a1+b1*xi;
    return ( (x1b-xi)*(xi-x1e)>=0 &&
             (x2b-xi)*(xi-x2e)>=0 &&
             (y1b-yi)*(yi-y1e)>=0 &&
             (y2b-yi)*(yi-y2e)>=0 );
}


