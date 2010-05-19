/****************************************************************************/
/// @file    Command_SaveTLSSwitchStates.h
/// @author  Daniel Krajzewicz
/// @date    08.05.2007
/// @version $Id$
///
// Writes the switch times of a tls into a file when the tls switches
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
#ifndef Command_SaveTLSSwitchStates_h
#define Command_SaveTLSSwitchStates_h


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
 * @class Command_SaveTLSSwitchStates
 * @brief Writes the switch times of a tls into a file when the tls switches
 *
 * @todo Revalidate this - as tls are not seting the link information directly ater being switched, the computed information may be delayed
 */
class Command_SaveTLSSwitchStates : public Command {
public:
    /** @brief Constructor
     *
     * @param[in] tlls The logic to write state of
     * @param[in] od The output device to write the state into
     */
    Command_SaveTLSSwitchStates(const MSTLLogicControl::TLSLogicVariants &logics,
                                OutputDevice &od) throw();


    /// @brief Destructor
    ~Command_SaveTLSSwitchStates() throw();


    /// @name Derived from Command
    /// @{

    /** @brief Writes the state of the tls if a change occured
     *
     * If the state or the active program has changed, the state is written
     *  to the output device.
     *
     * @param[in] currentTime The current simulation time
     * @return Always DELTA_T (will be executed in next time step)
     * @exception ProcessError not here
     * @see Command
     * @todo Here, a discrete even (on switch / program change) would be appropriate
     */
    SUMOTime execute(SUMOTime currentTime) throw(ProcessError);
    /// @}


private:
    /// @brief The device to write to
    OutputDevice &myOutputDevice;

    /// @brief The traffic light logic to use
    const MSTLLogicControl::TLSLogicVariants &myLogics;

    /// @brief Storage for prior state
    std::string myPreviousState;

    /// @brief Storage for prior sub-id
    std::string myPreviousProgramID;


private:
    /// @brief Invalidated copy constructor.
    Command_SaveTLSSwitchStates(const Command_SaveTLSSwitchStates&);

    /// @brief Invalidated assignment operator.
    Command_SaveTLSSwitchStates& operator=(const Command_SaveTLSSwitchStates&);

};


#endif

/****************************************************************************/

