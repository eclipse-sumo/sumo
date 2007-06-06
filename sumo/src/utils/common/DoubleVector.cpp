/****************************************************************************/
/// @file    DoubleVector.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A simple vector of SUMOReals
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "DoubleVector.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
SUMOReal
DoubleVectorHelper::sum(const DoubleVector &dv)
{
    SUMOReal sum = 0;
    for (DoubleVector::const_iterator i=dv.begin(); i!=dv.end(); i++) {
        sum += *i;
    }
    return sum;
}


void
DoubleVectorHelper::normalise(DoubleVector &dv, SUMOReal msum)
{
    if (msum==0) {
        // is an error; do nothing
        return;
    }
    SUMOReal rsum = sum(dv);
    if (rsum==0) {
        set(dv, (SUMOReal) 1.0/msum/(SUMOReal) dv.size());
        return;
    }
    div(dv, rsum/msum);
}


void
DoubleVectorHelper::div(DoubleVector &dv, SUMOReal by)
{
    for (DoubleVector::iterator i=dv.begin(); i!=dv.end(); i++) {
        *i /= by;
    }
}

void
DoubleVectorHelper::set(DoubleVector &dv, SUMOReal to)
{
    for (DoubleVector::iterator i=dv.begin(); i!=dv.end(); i++) {
        *i = to;
    }
}


void
DoubleVectorHelper::removeDouble(DoubleVector &v)
{
    DoubleVector::iterator i=v.begin();
    while (i!=v.end()) {
        for (DoubleVector::iterator j=i+1; j!=v.end();) {
            if (*i==*j) {
                j = v.erase(j);
            } else {
                j++;
            }
        }
        i++;
    }
}


SUMOReal
DoubleVectorHelper::maxValue(const DoubleVector &v)
{
    SUMOReal m = *(v.begin());
    for (DoubleVector::const_iterator j=v.begin()+1; j!=v.end(); j++) {
        if ((*j)>m) {
            m = *j;
        }
    }
    return m;
}


void
DoubleVectorHelper::remove_smaller_than(DoubleVector &v, SUMOReal swell)
{
    for (DoubleVector::iterator j=v.begin(); j!=v.end();) {
        if ((*j)<swell) {
            j = v.erase(j);
        } else {
            j++;
        }
    }
}


void
DoubleVectorHelper::remove_larger_than(DoubleVector &v, SUMOReal swell)
{
    for (DoubleVector::iterator j=v.begin(); j!=v.end();) {
        if ((*j)>swell) {
            j = v.erase(j);
        } else {
            j++;
        }
    }
}


SUMOReal
DoubleVectorHelper::minValue(const DoubleVector &v)
{
    SUMOReal m = *(v.begin());
    for (DoubleVector::const_iterator j=v.begin()+1; j!=v.end(); j++) {
        if ((*j)<m) {
            m = *j;
        }
    }
    return m;
}


void
DoubleVectorHelper::add2All(DoubleVector &v, SUMOReal what)
{
    for (DoubleVector::iterator j=v.begin(); j!=v.end(); j++) {
        (*j) += what;
    }
}



/****************************************************************************/

