//---------------------------------------------------------------------------//
//                        Distribution_Points.cpp -
//      The description of a distribution by a curve
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
// Revision 1.9  2006/04/07 05:30:53  dkrajzew
// distribution spread computation added (undocumented)
//
// Revision 1.8  2006/03/17 11:03:07  dkrajzew
// made access to positions in Position2DVector c++ compliant
//
// Revision 1.7  2005/10/07 11:43:51  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:06:37  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:17:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2004/01/28 12:35:22  dkrajzew
// retrival of a distribution maximum value added; documentation added
//
// Revision 1.3  2003/06/06 11:01:09  dkrajzew
// windows eol removed
//
// Revision 1.2  2003/06/05 14:33:44  dkrajzew
// class templates applied; documentation added
//
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

#include <cassert>
#include "Distribution.h"
#include <utils/geom/Position2DVector.h>
#include "Distribution_Points.h"
#include <utils/common/StdDefs.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
Distribution_Points::Distribution_Points(const std::string &id,
                                         const Position2DVector &points)
    : Distribution(id), myPoints(points), myProbabilitiesAreComputed(false)
{
}


Distribution_Points::~Distribution_Points()
{
}


SUMOReal
Distribution_Points::getMax() const
{
    assert(myPoints.size()>0);
    const Position2D &p = myPoints[-1];
    return p.x();
}


size_t
Distribution_Points::getAreaNo() const
{
    return myPoints.size()-1;
}


SUMOReal
Distribution_Points::getAreaBegin(size_t index) const
{
    return myPoints[index].x();
}


SUMOReal
Distribution_Points::getAreaEnd(size_t index) const
{
    return myPoints[index+1].x();
}


SUMOReal
Distribution_Points::getAreaPerc(size_t index) const
{
    if(!myProbabilitiesAreComputed) {
        SUMOReal sum = 0;
        size_t i;
        for(i=0; i<myPoints.size()-1; i++) {
            SUMOReal width = getAreaEnd(i) - getAreaBegin(i);
            SUMOReal minval = MIN2(myPoints[i].y(), myPoints[i].y());
            SUMOReal maxval = MAX2(myPoints[i].y(), myPoints[i].y());
            SUMOReal amount = minval * width + (maxval-minval) * width / (SUMOReal) 2.;
            myProbabilities.push_back(amount);
            sum += amount;
        }
        for(i=0; i<myPoints.size()-1; i++) {
            myProbabilities[i] = myProbabilities[i] / sum;
        }
        myProbabilitiesAreComputed = true;
    }
    return myProbabilities[index];
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


