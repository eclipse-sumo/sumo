/****************************************************************************/
/// @file    Command_SaveTLSSwitches.h
/// @author  Daniel Krajzewicz
/// @date    06 Jul 2006
/// @version $Id$
///
// Writes the switch times of a tls into a file
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
#ifndef Command_SaveTLSSwitches_h
#define Command_SaveTLSSwitches_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <utils/helpers/Command.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSTrafficLightLogic;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Command_SaveTLSSwitches
 * This action's execute command writes the state of the tls into a file
 */
class Command_SaveTLSSwitches : public Command
{
public:
    /// Constructor
    Command_SaveTLSSwitches(const MSTLLogicControl::TLSLogicVariants &logics,
                            OutputDevice *od);

    /// Destructor
    ~Command_SaveTLSSwitches();

    /// Executes the action
    SUMOTime execute(SUMOTime currentTime);

private:
    /// The device to write to
    OutputDevice *myOutputDevice;

    /// The traffic light logic to use
    const MSTLLogicControl::TLSLogicVariants &myLogics;

    /** @brief Storage for prior states
     *
     * A map from the link to the time it switched to green and whether
     *  the state change was saved
     */
    std::map<MSLink*, std::pair<SUMOTime, bool> > myPreviousLinkStates;

};


#endif

/****************************************************************************/

