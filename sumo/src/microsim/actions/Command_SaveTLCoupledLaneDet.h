/****************************************************************************/
/// @file    Command_SaveTLCoupledLaneDet.h
/// @author  Daniel Krajzewicz
/// @date    15 Feb 2004
/// @version $Id$
///
// Realises the output of a lane's detector values if the lane has green light
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
#ifndef Command_SaveTLCoupledLaneDet_h
#define Command_SaveTLCoupledLaneDet_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSLink.h>
#include "Command_SaveTLCoupledDet.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Command_SaveTLCoupledLaneDet
 * Called on every tls-switch, the "execute" checks whether the state of the
 *  assigned link is green. If so, the detector values of the assigned lane
 *  detectors are written to a file.
 * This action is build only if the user wants and describes it within the
 *  additional-files.
 */
class Command_SaveTLCoupledLaneDet : public Command_SaveTLCoupledDet
{
public:
    /// Constructor
    Command_SaveTLCoupledLaneDet(
        const MSTLLogicControl::TLSLogicVariants &tlls,
        MSDetectorFileOutput *dtf,
        unsigned int begin, OutputDevice& device, MSLink *link);

    /// Destructor
    ~Command_SaveTLCoupledLaneDet();

    /// Executes the command (see above)
    bool execute(SUMOTime currentTime);

private:
    /// The link to check
    MSLink *myLink;

    /// The state the link had the last time
    MSLink::LinkState myLastState;

};


#endif

/****************************************************************************/

