/****************************************************************************/
/// @file    MSSimpleTrafficLightLogic.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The basic traffic light logic
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
#ifndef MSSimpleTrafficLightLogic_h
#define MSSimpleTrafficLightLogic_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utility>
#include <vector>
#include <bitset>
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include "MSTrafficLightLogic.h"
#include "MSPhaseDefinition.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSSimpleTrafficLightLogic
 * The implementation of a simple traffic light which only switches between
 * it's phases and sets the lights to red in between.
 * Some functions are called with an information about the current step. This
 * is needed as a single logic may be used by many junctions and so the current
 * step is stored within them, not within the logic.
 */
class MSSimpleTrafficLightLogic : public MSTrafficLightLogic {
public:
    /// constructor
    MSSimpleTrafficLightLogic(MSNet &net, MSTLLogicControl &tlcontrol,
                              const std::string &id, const std::string &subid,
                              const Phases &phases, unsigned int step, SUMOTime delay);

    /// destructor
    ~MSSimpleTrafficLightLogic();

    /** @brief Switches to the next phase
        Returns the time of the next switch */
    SUMOTime trySwitch(bool isActive);



    /// @name Static Information Retrieval
    /// @{

    /** @brief Returns the number of phases
     * @return The number of this tls program's phases
     * @see MSTrafficLightLogic::getPhaseNumber
     */
    unsigned int getPhaseNumber() const throw();


    /** @brief Returns the phases of this tls program
     * @return The phases of this tls program
     * @see MSTrafficLightLogic::getPhases
     */
    const Phases &getPhases() const throw();


    /** @brief Returns the phases of this tls program
     * @return The phases of this tls program
     * @todo Recheck!
     */
    Phases &getPhases() throw();


    /** @brief Returns the definition of the phase from the given position within the plan
     * @param[in] givenstep The index of the phase within the plan
     * @return The definition of the phase at the given position
     * @see MSTrafficLightLogic::getPhase
     */
    const MSPhaseDefinition &getPhase(unsigned int givenstep) const throw();

    /// @}



    /// @name Dynamic Information Retrieval
    /// @{

    /** @brief Returns the current index within the program
     * @return The index of the current phase within the tls
     * @see MSTrafficLightLogic::getCurrentPhaseIndex
     */
    unsigned int getCurrentPhaseIndex() const throw();


    /** @brief Returns the definition of the current phase
     * @return The current phase
     * @see MSTrafficLightLogic::getCurrentPhaseDef
     */
    const MSPhaseDefinition &getCurrentPhaseDef() const throw();
    /// @}



    /// @name Conversion between time and phase
    /// @{

    /** @brief Returns the index of the logic at the given simulation step
     * @return The (estimated) index of the tls at the given simulation time step
     * @see MSTrafficLightLogic::getPhaseIndexAtTime
     */
    unsigned int getPhaseIndexAtTime(SUMOTime simStep) const throw();


    /** @brief Returns the position (start of a phase during a cycle) from of a given step
     * @param[in] index The index of the phase to return the begin of
     * @return The begin time of the phase
     * @see MSTrafficLightLogic::getOffsetFromIndex
     */
    unsigned int getOffsetFromIndex(unsigned int index) const throw();


    /** @brief Returns the step (the phasenumber) of a given position of the cycle
     * @param[in] offset The offset (time) for which the according phase shall be returned
     * @return The according phase
     * @see MSTrafficLightLogic::getIndexFromOffset
     */
    unsigned int getIndexFromOffset(unsigned int offset) const throw();
    /// @}


    void setLinkPriorities() const;
    bool maskRedLinks() const;

    /// returns the cycletime
    size_t getCycleTime() ;


    void changeStepAndDuration(MSTLLogicControl &tlcontrol, SUMOTime simStep,
                               unsigned int step, SUMOTime stepDuration);


protected:
    /// the list of phases this logic uses
    Phases myPhases;

    /// The current step
    unsigned int myStep;

    /// the cycletime
    size_t myCycleTime;

};


#endif

/****************************************************************************/

