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
// Revision 1.2  2005/01/27 14:22:45  dkrajzew
// ability to open the complete phase definition added; code style adapted
//
// Revision 1.1  2004/11/23 10:18:42  dkrajzew
// all traffic lights moved to microsim/traffic_lights
//
// Revision 1.3  2003/08/04 11:42:35  dkrajzew
// missing deletion of traffic light logics on closing a network added
//
// Revision 1.2  2003/07/30 09:16:10  dkrajzew
// a better (correct?) processing of yellow lights added; debugging
//
// Revision 1.1  2003/06/05 16:08:36  dkrajzew
// traffic lights are no longer junction-bound; a separate control is necessary
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
    for(LogicVector::iterator i=myLogics.begin(); i!=myLogics.end(); i++) {
        delete (*i);
    }
}


void
MSTLLogicControl::maskRedLinks()
{
    for(LogicVector::iterator i=myLogics.begin(); i!=myLogics.end(); i++) {
        (*i)->maskRedLinks();
    }
}


void
MSTLLogicControl::maskYellowLinks()
{
    for(LogicVector::iterator i=myLogics.begin(); i!=myLogics.end(); i++) {
        (*i)->maskYellowLinks();
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


