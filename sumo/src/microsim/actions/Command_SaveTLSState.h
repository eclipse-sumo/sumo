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
#include "Action.h"
#include <utils/helpers/Command.h>


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
    Command_SaveTLSState(MSTrafficLightLogic * const logic,
        const std::string &file);

    /// Destructor
    ~Command_SaveTLSState();

    /// Executes the action
    SUMOTime execute();

private:
    /// The file to write to
    std::ofstream myFile;

    /// The traffic light logic to use
    MSTrafficLightLogic *myLogic;

    /// The time this action is executed
    SUMOTime myExecTime;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
