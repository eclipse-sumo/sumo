//---------------------------------------------------------------------------//
//                        DoubleVector.cpp -
//  A simple vector of SUMOReals
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
// Revision 1.15  2005/10/07 11:43:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.14  2005/09/23 06:05:45  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.13  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.12  2005/04/28 09:02:46  dkrajzew
// level3 warnings removed
//
// Revision 1.11  2003/10/15 11:55:51  dkrajzew
// further work on vissim-import
//
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "DoubleVector.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
SUMOReal
DoubleVectorHelper::sum(const DoubleVector &dv)
{
    SUMOReal sum = 0;
    for(DoubleVector::const_iterator i=dv.begin(); i!=dv.end(); i++) {
        sum += *i;
    }
    return sum;
}


void
DoubleVectorHelper::normalise(DoubleVector &dv, SUMOReal msum)
{
    if(msum==0) {
        // is an error; do nothing
        return;
    }
    SUMOReal rsum = sum(dv);
    if(rsum==0) {
        set(dv, (SUMOReal) 1.0/msum/(SUMOReal) dv.size());
        return;
    }
    div(dv, rsum/msum);
}


void
DoubleVectorHelper::div(DoubleVector &dv, SUMOReal by)
{
    for(DoubleVector::iterator i=dv.begin(); i!=dv.end(); i++) {
        *i /= by;
    }
}

void
DoubleVectorHelper::set(DoubleVector &dv, SUMOReal to)
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


SUMOReal
DoubleVectorHelper::maxValue(const DoubleVector &v)
{
    SUMOReal m = *(v.begin());
    for(DoubleVector::const_iterator j=v.begin()+1; j!=v.end(); j++) {
        if((*j)>m) {
            m = *j;
        }
    }
    return m;
}


void
DoubleVectorHelper::remove_smaller_than(DoubleVector &v, SUMOReal swell)
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
DoubleVectorHelper::remove_larger_than(DoubleVector &v, SUMOReal swell)
{
    for(DoubleVector::iterator j=v.begin(); j!=v.end(); ) {
        if((*j)>swell) {
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
    for(DoubleVector::const_iterator j=v.begin()+1; j!=v.end(); j++) {
        if((*j)<m) {
            m = *j;
        }
    }
    return m;
}


void
DoubleVectorHelper::add2All(DoubleVector &v, SUMOReal what)
{
    for(DoubleVector::iterator j=v.begin(); j!=v.end(); j++) {
        (*j) += what;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


