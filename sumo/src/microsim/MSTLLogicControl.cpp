//---------------------------------------------------------------------------//
//                        MSTLLogicControl.cpp -
//  A class that holds all traffic light logics used
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
// Revision 1.1  2003/06/05 16:08:36  dkrajzew
// traffic lights are no longer junction-bound; a separate control is necessary
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include "MSTrafficLightLogic.h"
#include "MSTLLogicControl.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSTLLogicControl::MSTLLogicControl(const std::vector<MSTrafficLightLogic*>  &tlLogics)
    : myLogics(tlLogics)
{
}

MSTLLogicControl::~MSTLLogicControl()
{
}


void
MSTLLogicControl::maskRedLinks()
{
    for(LogicVector::iterator i=myLogics.begin(); i!=myLogics.end(); i++) {
        (*i)->maskRedLinks();
    }
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSTLLogicControl.icc"
//#endif

// Local Variables:
// mode:C++
// End:


