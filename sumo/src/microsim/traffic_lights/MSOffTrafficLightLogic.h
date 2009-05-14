/****************************************************************************/
/// @file    MSOffTrafficLightLogic.h
/// @author  Daniel Krajzewicz
/// @date    08.05.2007
/// @version $Id$
///
// A traffic lights logic which represents a tls in an off-mode
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
#ifndef MSOffTrafficLightLogic_h
#define MSOffTrafficLightLogic_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


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
 * @class MSOffTrafficLightLogic
 * @brief A traffic lights logic which represents a tls in an off-mode
 */
class MSOffTrafficLightLogic : public MSTrafficLightLogic {
public:
    /// constructor
    MSOffTrafficLightLogic(MSTLLogicControl &tlcontrol,
                           const std::string &id);


    /// Initialises the tls with information about incoming lanes
    virtual void init(NLDetectorBuilder &nb,
                      const MSEdgeContinuations &edgeContinuations);


    /** @brief Applies information about controlled links and lanes from the given logic
     * @param[in] logic The logic to use the information about controlled links/lanes from
     * @see MSTrafficLightLogic::adaptLinkInformationFrom
     */
    void adaptLinkInformationFrom(const MSTrafficLightLogic &logic);


    /// destructor
    ~MSOffTrafficLightLogic();

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



    /// Returns the priorities for all lanes for the current phase
    const std::bitset<64> &linkPriorities() const;

    const std::bitset<64> &allowed() const;

    /// returns the cycletime
    size_t getCycleTime() ;

    void setLinkPriorities() const;
    bool maskRedLinks() const;
    bool maskYellowLinks() const;
    void changeStepAndDuration(MSTLLogicControl &tlcontrol, SUMOTime simStep, unsigned int step, SUMOTime stepDuration) {
    }


private:
    /** @brief (Re)builds the internal phase definition
     */
    void rebuildPhase() throw();


private:
    /// @brief The phase definition (only one)
    MSTrafficLightLogic::Phases myPhaseDefinition;


};


#endif

/****************************************************************************/

