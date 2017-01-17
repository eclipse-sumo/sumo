/****************************************************************************/
/// @file    MSRailCrossing.h
/// @author  Jakob Erdmann
/// @date    Dez 2015
/// @version $Id$
///
// A rail signal logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSRailCrossing_h
#define MSRailCrossing_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSSimpleTrafficLightLogic.h"
#include "MSPhaseDefinition.h"
#include "MSTLLogicControl.h"



// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSRailCrossing
 * @brief A signal for rails
 */
class MSRailCrossing : public MSSimpleTrafficLightLogic {
public:
    /** @brief Constructor
     * @param[in] tlcontrol The tls control responsible for this tls
     * @param[in] id This tls' id
     * @param[in] subid This tls' sub-id (program id)
     * @param[in] parameters This tls' parameters
     */
    MSRailCrossing(MSTLLogicControl& tlcontrol,
                   const std::string& id, const std::string& subid,
                   const std::map<std::string, std::string>& parameters);


    /** @brief Initialises the rail signal with information about adjacent rail signals
     * @param[in] nb The detector builder
     * @exception ProcessError If something fails on initialisation
     */
    void init(NLDetectorBuilder& nb);


    /// @brief Destructor
    ~MSRailCrossing();

    /// @name Handling of controlled links
    /// @{

    /** @brief Adds a link on building
     * @param[in] link The controlled link
     * @param[in] lane The lane this link starts at
     * @param[in] pos The link's index (signal group) within this program
     */
    void addLink(MSLink* link, MSLane* lane, int pos);

    /** @brief Applies information about controlled links and lanes from the given logic
     * @param[in] logic The logic to use the information about controlled links/lanes from
     * @see MSTrafficLightLogic::adaptLinkInformationFrom
     */
    void adaptLinkInformationFrom(const MSTrafficLightLogic& logic);
    /// @}


    /// @name Switching and setting current rows
    /// @{


    /// @brief updates the current phase of the signal
    SUMOTime updateCurrentPhase();

    /** @brief Switches to the next phase
    * @return The time of the next switch (always the next step)
    * @see MSTrafficLightLogic::trySwitch
    */
    SUMOTime trySwitch();

    /// @}


    /// @name Static Information Retrieval
    /// @{
    /** @brief Returns the type of the logic as a string
     * @return The type of the logic
     */
    const std::string getLogicType() const {
        return "railCrossing";
    }
    /// @}


    /// @name Conversion between time and phase
    /// @{

    /** @brief Returns the index of the logic at the given simulation step
     * @return The (estimated) index of the tls at the given simulation time step (here, always zero will be returned)
     * @see MSTrafficLightLogic::getPhaseIndexAtTime
     */
    SUMOTime getPhaseIndexAtTime(SUMOTime simStep) const;


    /** @brief Returns the position (start of a phase during a cycle) from of a given step
     * @param[in] index The index of the phase to return the begin of
     * @return The begin time of the phase (here, always zero will be returned)
     * @see MSTrafficLightLogic::getOffsetFromIndex
     */
    SUMOTime getOffsetFromIndex(int index) const;


    /** @brief Returns the step (the phasenumber) of a given position of the cycle
    * @param[in] offset The offset (time) for which the according phase shall be returned
    * @return The according phase (here, always zero will be returned)
    * @see MSTrafficLightLogic::getIndexFromOffset
    */
    int getIndexFromOffset(SUMOTime offset) const;
    /// @}


    /// @name Changing phases and phase durations
    /// @{

    /** @brief Changes the current phase and her duration
     * @param[in] tlcontrol The responsible traffic lights control
     * @param[in] simStep The current simulation step
     * @param[in] step Index of the phase to use
     * @param[in] stepDuration The left duration of the phase
     * @see MSTrafficLightLogic::changeStepAndDuration
     */
    void changeStepAndDuration(MSTLLogicControl& tlcontrol, SUMOTime simStep, int step, SUMOTime stepDuration) {
        UNUSED_PARAMETER(tlcontrol);
        UNUSED_PARAMETER(simStep);
        UNUSED_PARAMETER(step);
        UNUSED_PARAMETER(stepDuration);
    }
    /// @}

protected:

    /// The incoming rail links
    std::vector<MSLink*> myIncomingRailLinks;

protected:


    /// @brief minimum time gap between closing the crossing (end of yellow time) and train passing the crossing
    SUMOTime mySecurityGap;

    /// @brief minimum green time
    SUMOTime myMinGreenTime;

    /// @brief minimum green time
    SUMOTime myYellowTime;

};


#endif

/****************************************************************************/

