/****************************************************************************/
/// @file    MSRailSignal.h
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Jan 2015
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
#ifndef MSRailSignal_h
#define MSRailSignal_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

//#include <utility>
//#include <vector>
//#include <bitset>
//#include <map>
//#include <microsim/MSEventControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
//#include "MSSimpleTrafficLightLogic.h"
#include "MSPhaseDefinition.h"
#include "MSTLLogicControl.h"
//#include <microsim/output/MSInductLoop.h>



// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSRailSignal
 * @brief A signal for rails
 */
class MSRailSignal : public MSTrafficLightLogic {
public:
    /** @brief Constructor
     * @param[in] tlcontrol The tls control responsible for this tls
     * @param[in] id This tls' id
     * @param[in] subid This tls' sub-id (program id)
     * @param[in] parameters This tls' parameters
     */
    MSRailSignal(MSTLLogicControl& tlcontrol,
                 const std::string& id, const std::string& subid,
                 const std::map<std::string, std::string>& parameters);


    /** @brief Initialises the rail signal with information about adjacent rail signals
     * @param[in] nb The detector builder
     * @exception ProcessError If something fails on initialisation
     */
    void init(NLDetectorBuilder& nb);


    /// @brief Destructor
    ~MSRailSignal();

    /// @name Handling of controlled links
    /// @{

    /** @brief Applies information about controlled links and lanes from the given logic
     * @param[in] logic The logic to use the information about controlled links/lanes from
     * @see MSTrafficLightLogic::adaptLinkInformationFrom
     */
    void adaptLinkInformationFrom(const MSTrafficLightLogic& logic);
    /// @}


    /// @name Switching and setting current rows
    /// @{


    /** @brief returns the state of the signal that actually required
     *
     * Returns the state of the rail signal that is actually required based on the
     *  occupation of the adjoining blocks.
     *
     * @return The state actually required for this signal.
     */
    std::string getAppropriateState();

    /// @brief updates the current phase of the signal
    void updateCurrentPhase();

    /** @brief Switches to the next phase
    * @return The time of the next switch (always the next step)
    * @see MSTrafficLightLogic::trySwitch
    */
    SUMOTime trySwitch();

    /// @}


    /// @name Static Information Retrieval
    /// @{

    /** @brief Returns the number of phases
     * @return The number of this tls program's phases (always zero)
     * @see MSTrafficLightLogic::getPhaseNumber
     */
    int getPhaseNumber() const;


    /** @brief Returns the phases of this tls program
     * @return The phases of this tls program
     * @see MSTrafficLightLogic::getPhases
     */
    const Phases& getPhases() const;

    /** @brief Returns the definition of the phase from the given position within the plan
    *
    * Returns the current phase as there does not exist a plan of the phases.
    *
    * @param[in] givenstep The index of the phase within the plan
    * @return The definition of the phase at the given position
    * @see MSTrafficLightLogic::getPhase
    */
    const MSPhaseDefinition& getPhase(int givenstep) const;

    /** @brief Returns the type of the logic as a string
     * @return The type of the logic
     */
    const std::string getLogicType() const {
        return "railSignal";
    }
    /// @}


    /// @name Dynamic Information Retrieval
    /// @{

    /** @brief Returns the current index within the program
     * @return The index of the current phase within the tls (here, always zero will be returned)
     * @see MSTrafficLightLogic::getCurrentPhaseIndex
     */
    int getCurrentPhaseIndex() const;


    /** @brief Returns the definition of the current phase
    * @return The current phase
    */
    const MSPhaseDefinition& getCurrentPhaseDef() const;
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

    /// The set of lanes going out from the junction
    std::vector<MSLane*> myOutgoingLanes;

    /// A map that maps a link to its link index
    std::map<MSLink*, int> myLinkIndices;

    /// A map that maps an outgoing lane from the junction  to its set of links that lead to this lane
    std::map<MSLane*, std::vector<MSLink*> > myLinksToLane;

    /// A map that maps a link from the junction to its vector of lanes leading from a previous signal to this link
    std::map<MSLink*, std::vector<const MSLane*> > myAfferentBlocks;

    /// A map that maps an outgoing lane from the junction to its vector of lanes leading to the next signal
    std::map<MSLane*, std::vector<const MSLane*> > mySucceedingBlocks;


protected:

    /** @brief The list of phases this logic uses
    *
    *   This vector is always empty and only constructed because the superclass MSTrafficLightLogic requires it.
    */
    Phases myPhases;

    /// @brief The current phase
    MSPhaseDefinition myCurrentPhase;

};


#endif

/****************************************************************************/

