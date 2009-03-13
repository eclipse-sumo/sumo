/****************************************************************************/
/// @file    Command_SaveTLSSwitches.h
/// @author  Daniel Krajzewicz
/// @date    06 Jul 2006
/// @version $Id$
///
// Writes information about the green durations of a tls
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
 * @class Command_SaveTLSSwitches
 * @brief Writes information about the green durations of a tls
 *
 * @todo Revalidate this - as tls are not seting the link information directly ater being switched, the computed information may be delayed
 */
class Command_SaveTLSSwitches : public Command {
public:
    /** @brief Constructor
     *
     * @param[in] tlls The logic to write state of
     * @param[in] od The output device to write the state into
     */
    Command_SaveTLSSwitches(const MSTLLogicControl::TLSLogicVariants &logics,
                            OutputDevice &od) throw();


    /// @brief Destructor
    ~Command_SaveTLSSwitches() throw();


    /// @name Derived from Command
    /// @{

    /** @brief Writes the output if a change occured
     *
     * Called in each tme step, this class computes which link have red
     *  since the last tls switch and writes the information about their
     *  green duration into the given stream.
     *
     * Information whether a link had green and since when is stored in
     *  "myPreviousLinkStates".
     *
     * @param[in] currentTime The current simulation time
     * @return Always 1 (will be executed in next time step)
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

    /** @brief Storage for prior states
     *
     * A map from the link to the time it switched to green and whether the state change was saved
     */
    std::map<MSLink*, std::pair<SUMOTime, bool> > myPreviousLinkStates;


private:
    /// @brief Invalidated copy constructor.
    Command_SaveTLSSwitches(const Command_SaveTLSSwitches&);

    /// @brief Invalidated assignment operator.
    Command_SaveTLSSwitches& operator=(const Command_SaveTLSSwitches&);

};


#endif

/****************************************************************************/

