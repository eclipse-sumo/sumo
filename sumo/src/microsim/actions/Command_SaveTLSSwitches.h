#ifndef Command_SaveTLSSwitches_h
#define Command_SaveTLSSwitches_h
//---------------------------------------------------------------------------//
//                        Command_SaveTLSSwitches.h -
//  Writes the switch times of a tls into a file
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 06 Jul 2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
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
// Revision 1.1  2006/07/06 12:22:06  dkrajzew
// tls switches added
//
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
 * @class Command_SaveTLSSwitches
 * This action's execute command writes the state of the tls into a file
 */
class Command_SaveTLSSwitches : public Command
{
public:
    /// Constructor
    Command_SaveTLSSwitches(const MSTLLogicControl::TLSLogicVariants &logics,
        const std::string &file);

    /// Destructor
    ~Command_SaveTLSSwitches();

    /// Executes the action
    SUMOTime execute(SUMOTime currentTime);

private:
    /// The file to write to
    std::ofstream myFile;

    /// The traffic light logic to use
    const MSTLLogicControl::TLSLogicVariants &myLogics;

    /** @brief Storage for prior states
     *
     * A map from the link to the time it switched to green and whether
     *  the state change was saved
     */
    std::map<MSLink*, std::pair<SUMOTime, bool> > myPreviousLinkStates;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
