/****************************************************************************/
/// @file    MSTrafficLightLogic.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The parent class for traffic light logics
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
#ifndef MSTrafficLightLogic_h
#define MSTrafficLightLogic_h
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

#include <map>
#include <string>
#include <bitset>
#include <utils/helpers/Command.h>
#include <microsim/MSLogicJunction.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLink;
class MSEventControl;
class DiscreteCommand;
class MSTLLogicControl;
class NLDetectorBuilder;
class MSEdgeContinuations;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTrafficLightLogic
 */
class MSTrafficLightLogic
{
public:
    /// Definition of the list of links that participate in this tl-light
    typedef std::vector<MSLink*> LinkVector;

    /// Definition of a list that holds lists of links that do have the same attribute
    typedef std::vector<LinkVector> LinkVectorVector;

    /// Definition of the list of links that participate in this tl-light
    typedef std::vector<MSLane*> LaneVector;

    /// Definition of a list that holds lists of links that do have the same attribute
    typedef std::vector<LaneVector> LaneVectorVector;

public:
    /// Constructor
    MSTrafficLightLogic(MSTLLogicControl &tlcontrol,
                        const std::string &id, const std::string &subid, size_t delay);

    /// Destructor
    virtual ~MSTrafficLightLogic();

    /** @brief Switches to the next phase
        Returns the time of the next switch */
    virtual SUMOTime trySwitch(bool isActive) = 0;

    /** Returns the link priorities for the given phase */
    virtual const std::bitset<64> &linkPriorities() const = 0;

    /// Returns the mask of links that have yellow
    virtual const std::bitset<64> &yellowMask() const = 0;

    /// Returns the mask of links that may move
    virtual const std::bitset<64> &allowed() const = 0;

    /// Returns the current step
    virtual size_t getStepNo() const = 0;


    /** @brief Sets the priorities of incoming lanes
        This must be done as they change when the light changes */
    void setLinkPriorities();

    /// Clears all incoming vehicle information on links that have red
    bool maskRedLinks();

    /// Clears all incoming vehicle information on links that have yellow
    bool maskYellowLinks();

    friend class NLSucceedingLaneBuilder;

    /// Builds a string that contains the states of the signals
    std::string buildStateList() const;

    /// Returns the list of lanes that are controlled by the signals at the given position
    const LaneVector &getLanesAt(size_t i) const;

    /// Returns the list of lists of all lanes controlled by this tls
    const LaneVectorVector &getLanes() const;

    /// Returns the list of links that are controlled by the signals at the given position
    const LinkVector &getLinksAt(size_t i) const;

    /// Returns all affected links
    const LinkVectorVector &getLinks() const;

    /// Returns this tl-logic's id
    const std::string &getID() const;

    /// Returns this tl-logic's id
    const std::string &getSubID() const;

    /// Adds an action that shall be executed if the tls switched
    void addSwitchAction(DiscreteCommand *a);

    /// Executes commands if the tls switched (!!! should be protected/private)
    void onSwitch();

    void adaptLinkInformationFrom(const MSTrafficLightLogic &logic);

    void setParameter(const std::map<std::string, std::string> &params);

    std::string getParameterValue(const std::string &key) const;

    void addOverridingDuration(SUMOTime duration);
    void setCurrentDurationIncrement(SUMOTime delay);
    virtual void changeStepAndDuration(MSTLLogicControl &tlcontrol,
                                       SUMOTime simStep, int step, SUMOTime stepDuration) = 0;

    virtual void init(NLDetectorBuilder &nb,
                      const MSEdgeContinuations &edgeContinuations);

    /// Returns the index of the given link
    int getLinkIndex(MSLink *link) const;


protected:
    /// Adds a link on building
    void addLink(MSLink *link, MSLane *lane, size_t pos);

    /**
     * @class SwitchCommand
     * Class realising the switch between the traffic light states (phases
     */
class SwitchCommand : public Command
    {
    public:
        /// Constructor
        SwitchCommand(MSTLLogicControl &tlcontrol,
                      MSTrafficLightLogic *tlLogic);

        /// Destructor
        ~SwitchCommand();

        /** @brief Executes this event
            Executes the regarded junction's "trySwitch"- method */
        SUMOTime execute(SUMOTime currentTime);

        void deschedule(MSTrafficLightLogic *tlLogic);

    private:
        MSTLLogicControl &myTLControl;

        /// The logic to be executed on a switch
        MSTrafficLightLogic *myTLLogic;

        /// Information whether this switch command is still valid
        bool myAmValid;

    };

protected:
    /// given parameter
    std::map<std::string, std::string> myParameter;

    /// The id of the logic
    std::string myID, mySubID;

    /// The list of links which do participate in this traffic light
    LinkVectorVector myLinks;

    /// The list of links which do participate in this traffic light
    LaneVectorVector myLanes;

    /// The list of actions/commands to execute on switch
    std::vector<DiscreteCommand*> myOnSwitchActions;

    /// A list of duration overrides
    std::vector<SUMOTime> myOverridingTimes;

    /// A value for enlarge the current duration
    SUMOTime myCurrentDurationIncrement;

    /// The current switch command
    SwitchCommand *mySwitchCommand;

private:
    /// invalidated copy constructor
    MSTrafficLightLogic(const MSTrafficLightLogic &s);

    /// invalidated assignment operator
    MSTrafficLightLogic& operator=(const MSTrafficLightLogic &s);

};


#endif

/****************************************************************************/

