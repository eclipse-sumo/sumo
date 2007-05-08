/****************************************************************************/
/// @file    Command_SaveTLSSwitchStates.h
/// @author  Daniel Krajzewicz
/// @date    08.05.2007
/// @version $Id: Command_SaveTLSSwitchStates.h 3829 2007-04-19 09:28:31 +0200 (Do, 19 Apr 2007) dkrajzew $
///
// Writes the state of the tls to a file when it switches
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
#ifndef Command_SaveTLSSwitchStates_h
#define Command_SaveTLSSwitchStates_h
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
#include <fstream>
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
 * @class Command_SaveTLSSwitchStates
 * @brief Writes the state of the tls to a file when it switches
 */
class Command_SaveTLSSwitchStates : public Command
{
public:
    /// Constructor
    Command_SaveTLSSwitchStates(const MSTLLogicControl::TLSLogicVariants &logics,
                         OutputDevice *od);

    /// Destructor
    ~Command_SaveTLSSwitchStates();

    /// Executes the action
    SUMOTime execute(SUMOTime currentTime);

private:
    /// The device to write to
    OutputDevice *myOutputDevice;

    /// The traffic light logic to use
    const MSTLLogicControl::TLSLogicVariants &myLogics;

    /** @brief Storage for prior state */
    std::string myPreviousState;

    /** @brief Storage for prior sub-id */
    std::string myPreviousSubID;

};


#endif

/****************************************************************************/

