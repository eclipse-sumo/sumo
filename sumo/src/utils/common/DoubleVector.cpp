//---------------------------------------------------------------------------//
//                        DoubleVector.cpp -
//  A simple vector of doubles
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.10  2003/09/05 15:25:35  dkrajzew
// solved some naming problems
//
// Revision 1.9  2003/08/20 11:46:27  dkrajzew
// some further methods added needed for the computation of node shapes
//
// Revision 1.8  2003/07/21 05:16:08  dkrajzew
// inifinite loop patched; one should make both classes a single templte!
//
// Revision 1.7  2003/07/18 12:35:06  dkrajzew
// removed some warnings
//
// Revision 1.6  2003/06/05 11:54:48  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "DoubleVector.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
double
DoubleVectorHelper::sum(const DoubleVector &dv)
{
    double sum = 0;
    for(DoubleVector::const_iterator i=dv.begin(); i!=dv.end(); i++) {
        sum += *i;
    }
    return sum;
}


void
DoubleVectorHelper::normalise(DoubleVector &dv, double msum)
{
    if(msum==0) {
        // is an error; do nothing
        return;
    }
    double rsum = sum(dv);
    if(rsum==0) {
        set(dv, 1.0/msum/(double) dv.size());
        return;
    }
    div(dv, rsum/msum);
}


void
DoubleVectorHelper::div(DoubleVector &dv, double by)
{
    for(DoubleVector::iterator i=dv.begin(); i!=dv.end(); i++) {
        *i /= by;
    }
}

void
DoubleVectorHelper::set(DoubleVector &dv, double to)
{
    for(DoubleVector::iterator i=dv.begin(); i!=dv.end(); i++) {
        *i = to;
    }
}


void
DoubleVectorHelper::removeDouble(DoubleVector &v)
{
    size_t pos = 0;
    DoubleVector::iterator i=v.begin();
    while(i!=v.end()) {
        for(DoubleVector::iterator j=i+1; j!=v.end(); ) {
            if(*i==*j) {
                j = v.erase(j);
            } else {
                j++;
            }
        }
        i++;
    }
}


double
DoubleVectorHelper::maxValue(const DoubleVector &v)
{
    double m = *(v.begin());
    for(DoubleVector::const_iterator j=v.begin()+1; j!=v.end(); j++) {
        if((*j)>m) {
            m = *j;
        }
    }
    return m;
}


void
DoubleVectorHelper::remove_smaller_than(DoubleVector &v, double swell)
{
    for(DoubleVector::iterator j=v.begin(); j!=v.end(); ) {
        if((*j)<swell) {
            j = v.erase(j);
        } else {
            j++;
        }
    }
}


void
DoubleVectorHelper::remove_larger_than(DoubleVector &v, double swell)
{
    for(DoubleVector::iterator j=v.begin(); j!=v.end(); ) {
        if((*j)>swell) {
            j = v.erase(j);
        } else {
            j++;
        }
    }
}


double
DoubleVectorHelper::minValue(const DoubleVector &v)
{
    double m = *(v.begin());
    for(DoubleVector::const_iterator j=v.begin()+1; j!=v.end(); j++) {
        if((*j)<m) {
            m = *j;
        }
    }
    return m;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "DoubleVector.icc"
//#endif

// Local Variables:
// mode:C++
// End:


