//---------------------------------------------------------------------------//
//                        GUILaneStateBounderiesStorage.cpp -
//  A class that stores the min and max-bounderies of lane state values
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 20th Oct 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.3  2003/12/04 13:36:09  dkrajzew
// work on setting of aggregated value bounderies
//
// Revision 1.2  2003/11/24 10:16:08  dkrajzew
// documentation added; division by zero-error patched
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUILaneStateBounderiesStorage.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUILaneStateBounderiesStorage::GUILaneStateBounderiesStorage()
    : myAmGlobalInitialised(false), myAmStepInitialised(false)
{
}


GUILaneStateBounderiesStorage::~GUILaneStateBounderiesStorage()
{
}


void
GUILaneStateBounderiesStorage::addValue(E2::DetType type,
                                        double val)
{
    // for all-the-time values
    if(!myAmGlobalInitialised) {
        myAmGlobalInitialised = true;
        myTimeGlobalMaxes[type] = val;
        myTimeGlobalMins[type] = val;
        myTimeGlobalRanges[type] = 0;
    } else {
        if(myTimeGlobalMaxes[type]<val) {
            myTimeGlobalMaxes[type] = val;
            myTimeGlobalRanges[type] = val - myTimeGlobalMins[type];
        }
        if(myTimeGlobalMins[type]>val) {
            myTimeGlobalMins[type] = val;
            myTimeGlobalRanges[type] = myTimeGlobalMaxes[type] - val;
        }
    }
    // for step values
    if(!myAmStepInitialised) {
        myAmStepInitialised = true;
        myTimeStepMaxes[type] = val;
        myTimeStepMins[type] = val;
        myTimeStepRanges[type] = 0;
    } else {
        if(myTimeStepMaxes[type]<val) {
            myTimeStepMaxes[type] = val;
            myTimeStepRanges[type] = val - myTimeStepMins[type];
        }
        if(myTimeStepMins[type]>val) {
            myTimeStepMins[type] = val;
            myTimeStepRanges[type] = myTimeStepMaxes[type] - val;
        }
    }
}


double
GUILaneStateBounderiesStorage::timeStepNorm(double val,
                                            E2::DetType type)
{
    // check whether any values were inserted, yet
    if(!myAmStepInitialised) {
        // return mean if not
        return 0.5;
    }
    // check whether a range exists
    double range = myTimeStepRanges[type];
    if(range==0) {
        // return mean if not
        return 0.5;
    }
    // compute the normalised value
    val -= myTimeStepMins[type];
    return val /= range;
}


void
GUILaneStateBounderiesStorage::initStep()
{
    myAmStepInitialised = false;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


