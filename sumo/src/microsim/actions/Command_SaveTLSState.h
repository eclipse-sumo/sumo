/****************************************************************************/
/// @file    Command_SaveTLSState.h
/// @author  Daniel Krajzewicz
/// @date    15 Feb 2004
/// @version $Id$
///
// Writes the state of the tls to a file (in each second)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef Command_SaveTLSState_h
#define Command_SaveTLSState_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <utils/common/Command.h>
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
 * @class Command_SaveTLSState
 * @brief Writes the state of the tls to a file (in each second)
 */
class Command_SaveTLSState : public Command {
public:
    /** @brief Constructor
     *
     * @param[in] tlls The logic to write state of
     * @param[in] od The output device to write the state into
     */
    Command_SaveTLSState(const MSTLLogicControl::TLSLogicVariants &logics,
                         OutputDevice &od) throw();


    /// @brief Destructor
    ~Command_SaveTLSState() throw();


    /// @name Derived from Command
    /// @{

    /** @brief Writes the current state of the tls
     *
     * Returns always 1.
     *
     * @param[in] currentTime The current simulation time (unused)
     * @return Always DELTA_T (will be executed in next time step)
     * @exception ProcessError not here
     * @see Command
     */
    SUMOTime execute(SUMOTime currentTime) throw(ProcessError);
    /// @}


private:
    /// @brief The device to write to
    OutputDevice &myOutputDevice;

    /// @brief The traffic light logic to use
    const MSTLLogicControl::TLSLogicVariants &myLogics;


private:
    /// @brief Invalidated copy constructor.
    Command_SaveTLSState(const Command_SaveTLSState&);

    /// @brief Invalidated assignment operator.
    Command_SaveTLSState& operator=(const Command_SaveTLSState&);

};


#endif

/****************************************************************************/

