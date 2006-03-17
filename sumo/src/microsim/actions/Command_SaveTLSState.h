#ifndef Command_SaveTLSState_h
#define Command_SaveTLSState_h
//---------------------------------------------------------------------------//
//                        Command_SaveTLSState.h -
//  Writes the state of the tls to a file
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 15 Feb 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.8  2006/03/17 08:53:17  dkrajzew
// "Action" removed - was the same as Command; changed the Event-interface (execute now gets the current simulation time)
//
// Revision 1.7  2006/02/27 12:03:23  dkrajzew
// variants container named properly
//
// Revision 1.6  2006/02/23 11:27:57  dkrajzew
// tls may have now several programs
//
// Revision 1.5  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 11:07:14  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/02/01 10:10:43  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.2  2004/02/16 14:02:57  dkrajzew
// e2-link-dependent detectors added
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

#include <string>
#include <fstream>
#include <map>
#include <utils/helpers/Command.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class FileWriter;
class MSTrafficLightLogic;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class Command_SaveTLSState
 * This action's execute command writes the state of the tls into a file
 */
class Command_SaveTLSState : public Command
{
public:
    /// Constructor
    Command_SaveTLSState(const MSTLLogicControl::TLSLogicVariants &logics,
        const std::string &file);

    /// Destructor
    ~Command_SaveTLSState();

    /// Executes the action
    SUMOTime execute(SUMOTime currentTime);

private:
    /// The file to write to
    std::ofstream myFile;

    /// The traffic light logic to use
    const MSTLLogicControl::TLSLogicVariants &myLogics;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
