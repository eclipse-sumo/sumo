/****************************************************************************/
/// @file    MSTrafficLightLogic.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The parent class for traffic light logics
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
#ifndef MSTrafficLightLogic_h
#define MSTrafficLightLogic_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>
#include <bitset>
#include <utils/common/Command.h>
#include <microsim/MSLogicJunction.h>
#include <microsim/MSLink.h>
#include "MSPhaseDefinition.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLink;
class MSTLLogicControl;
class NLDetectorBuilder;
class MSEdgeContinuations;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTrafficLightLogic
 * @brief The parent class for traffic light logics
 */
class MSTrafficLightLogic {
public:
    /// @name Structure definitions
    /// @{

    /// @brief Definition of a list of phases, being the junction logic
    typedef std::vector<MSPhaseDefinition*> Phases;

    /// @brief Definition of the list of links that participate in this tl-light
    typedef std::vector<MSLink*> LinkVector;

    /// @brief Definition of a list that holds lists of links that do have the same attribute
    typedef std::vector<LinkVector> LinkVectorVector;

    /// @brief Definition of the list of links that participate in this tl-light
    typedef std::vector<MSLane*> LaneVector;

    /// @brief Definition of a list that holds lists of links that do have the same attribute
    typedef std::vector<LaneVector> LaneVectorVector;
    /// @}


public:
    /** @brief Constructor
     * @param[in] tlcontrol The tls control responsible for this tls 
     * @param[in] id This tls' id
     * @param[in] subid This tls' sub-id (program id)
     * @param[in] delay The time to wait before the first switch
     */
    MSTrafficLightLogic(MSTLLogicControl &tlcontrol,
                        const std::string &id, const std::string &subid, SUMOTime delay);


    /** @brief Initialises the tls with information about incoming lanes
     * @param[in] nb The detector builder
     * @param[in] edgeContinuations Information about edge predecessors/successors
     */
    virtual void init(NLDetectorBuilder &nb,
                      const MSEdgeContinuations &edgeContinuations);


    /** @brief Applies information about controlled links and lanes from the given logic
     *
     * If we load a logic after the network has been loaded, we do not get the information
     *  about controlled links afterwards. We have to copy them from a previously loaded logic.
     *
     * @param[in] logic The logic to use the information about controlled links/lanes from
     */
    virtual void adaptLinkInformationFrom(const MSTrafficLightLogic &logic);


    /// @brief Destructor
    virtual ~MSTrafficLightLogic();


    /** @brief Switches to the next phase
        Returns the time of the next switch */
    virtual SUMOTime trySwitch(bool isActive) = 0;



    /// @name Static Information Retrieval
    /// @{

    /** @brief Returns this tl-logic's id
     * @return This tls' id
     */
    const std::string &getID() const throw() {
        return myID;
    }


    /** @brief Returns this tl-logic's id
     * @return This program's id
     */
    const std::string &getSubID() const throw() {
        return mySubID;
    }


    /** @brief Returns the list of lists of all lanes controlled by this tls
     * @return All lanes controlled by this tls, sorted by the signal index
     */
    const LaneVectorVector &getLanes() const throw() {
        return myLanes;
    }


    /** @brief Returns the list of lanes that are controlled by the signals at the given position
     * @param[in] i The index of the signal
     * @return The lanes controlled by the signal at the given index
     */
    const LaneVector &getLanesAt(unsigned int i) const throw() {
        return myLanes[i];
    }


    /** @brief Returns the list of lists of all affected links
     * @return All links controlled by this tls, sorted by the signal index
     */
    const LinkVectorVector &getLinks() const throw() {
        return myLinks;
    }


    /** @brief Returns the list of links that are controlled by the signals at the given position
     * @param[in] i The index of the signal
     * @return The links controlled by the signal at the given index
     */
    const LinkVector &getLinksAt(unsigned int i) const throw() {
        return myLinks[i];
    }


    /** @brief Returns the index of the given link
     * @param[in] link The link to retrieve the index for
     * @return The index of the given link (-1 if it is not controlled by this tls)
     */
    int getLinkIndex(const MSLink * const link) const throw();


    /** @brief Returns the number of phases
     * @return The number of this tls program's phases
     */
    virtual unsigned int getPhaseNumber() const throw() = 0;


    /** @brief Returns the phases of this tls program
     * @return The phases of this tls program
     */
    virtual const Phases &getPhases() const throw() = 0;


    /** @brief Returns the definition of the phase from the given position within the plan
     * @param[in] givenstep The index of the phase within the plan
     * @return The definition of the phase at the given position
     */
    virtual const MSPhaseDefinition &getPhase(unsigned int givenstep) const throw() = 0;
    /// @}



    /// @name Dynamic Information Retrieval
    /// @{

    /** @brief Returns the current index within the program
     * @return The index of the current phase within the tls
     */
    virtual unsigned int getCurrentPhaseIndex() const throw() = 0;


    /** @brief Returns the definition of the current phase
     * @return The current phase
     */
    virtual const MSPhaseDefinition &getCurrentPhaseDef() const throw() = 0;
    /// @}



    /// @name Conversion between time and phase
    /// @{
    /** @brief Returns the index of the logic at the given simulation step
     * @return The (estimated) index of the tls at the given simulation time step
     */
    virtual unsigned int getPhaseIndexAtTime(SUMOTime simStep) const throw() = 0;


    /** @brief Returns the position (start of a phase during a cycle) from of a given step
     * @param[in] index The index of the phase to return the begin of
     * @return The begin time of the phase
     */
    virtual unsigned int getOffsetFromIndex(unsigned int index) const throw() = 0;


    /** @brief Returns the step (the phasenumber) of a given position of the cycle
     * @param[in] offset The offset (time) for which the according phase shall be returned
     * @return The according phase
     */
    virtual unsigned int getIndexFromOffset(unsigned int offset) const throw() = 0;
    /// @}



    /** @brief Sets the priorities of incoming lanes
        This must be done as they change when the light changes */
    virtual void setLinkPriorities() const = 0;

    /// Clears all incoming vehicle information on links that have red
    virtual bool maskRedLinks() const = 0;




    void setParameter(const std::map<std::string, std::string> &params);

    std::string getParameterValue(const std::string &key) const;

    void addOverridingDuration(SUMOTime duration);
    void setCurrentDurationIncrement(SUMOTime delay);
    virtual void changeStepAndDuration(MSTLLogicControl &tlcontrol,
                                       SUMOTime simStep, unsigned int step, SUMOTime stepDuration) = 0;


    std::map<MSLink*, std::pair<MSLink::LinkState, bool> > collectLinkStates() const;
    void resetLinkStates(const std::map<MSLink*, std::pair<MSLink::LinkState, bool> > &vals) const;


    /// Adds a link on building
    void addLink(MSLink *link, MSLane *lane, size_t pos);

protected:

    /**
     * @class SwitchCommand
     * Class realising the switch between the traffic light states (phases
     */
    class SwitchCommand : public Command {
    public:
        /// Constructor
        SwitchCommand(MSTLLogicControl &tlcontrol,
                      MSTrafficLightLogic *tlLogic) throw();

        /// Destructor
        ~SwitchCommand() throw();

        /** @brief Executes this event
            Executes the regarded junction's "trySwitch"- method */
        SUMOTime execute(SUMOTime currentTime) throw(ProcessError);

        void deschedule(MSTrafficLightLogic *tlLogic);

    private:
        MSTLLogicControl &myTLControl;

        /// The logic to be executed on a switch
        MSTrafficLightLogic *myTLLogic;

        /// Information whether this switch command is still valid
        bool myAmValid;

    private:
        /// @brief Invalidated copy constructor.
        SwitchCommand(const SwitchCommand&);

        /// @brief Invalidated assignment operator.
        SwitchCommand& operator=(const SwitchCommand&);

    };

protected:
    /// @brief Given parameter
    std::map<std::string, std::string> myParameter;

    /// @brief The id of the logic
    std::string myID, mySubID;

    /// @brief The list of links which do participate in this traffic light
    LinkVectorVector myLinks;

    /// @brief The list of links which do participate in this traffic light
    LaneVectorVector myLanes;

    /// @brief A list of duration overrides
    std::vector<SUMOTime> myOverridingTimes;

    /// @brief A value for enlarge the current duration
    SUMOTime myCurrentDurationIncrement;

    /// @brief The current switch command
    SwitchCommand *mySwitchCommand;

private:
    /// @brief invalidated copy constructor
    MSTrafficLightLogic(const MSTrafficLightLogic &s);

    /// @brief invalidated assignment operator
    MSTrafficLightLogic& operator=(const MSTrafficLightLogic &s);

};


#endif

/****************************************************************************/

