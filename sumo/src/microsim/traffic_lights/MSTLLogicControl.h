#ifndef MSTLLogicControl_h
#define MSTLLogicControl_h
//---------------------------------------------------------------------------//
//                        MSTLLogicControl.h -
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
// $Log$
// Revision 1.2  2005/01/27 14:22:45  dkrajzew
// ability to open the complete phase definition added; code style adapted
//
// Revision 1.1  2004/11/23 10:18:42  dkrajzew
// all traffic lights moved to microsim/traffic_lights
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


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSTrafficLightLogic;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSTLLogicControl {
public:
    /// Constructor
    MSTLLogicControl(const std::vector<MSTrafficLightLogic*> &tlLogics);

    /// Destructor
    ~MSTLLogicControl();

    /// For all traffic lights, the requests are masked away if they have red light (not yellow)
    void maskRedLinks();

    /// For all traffic lights, the requests are masked away if they have yellow light
    void maskYellowLinks();

private:
    /// Definition of the storage for traffic light logics
    typedef std::vector<MSTrafficLightLogic*> LogicVector;

    /// The list of traffic light logics
    LogicVector myLogics;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

