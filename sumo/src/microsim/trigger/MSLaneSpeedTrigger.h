/****************************************************************************/
/// @file    MSLaneSpeedTrigger.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Changes the speed allowed on a set of lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSLaneSpeedTrigger_h
#define MSLaneSpeedTrigger_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/common/Command.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include "MSTrigger.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLaneSpeedTrigger
 * @brief Changes the speed allowed on a set of lanes
 *
 * This trigger reads the next maximum velocity of the lane he is
 * responsible for from a file and sets it.
 * Lanes with variable speeds are so possible.
 */
class MSLaneSpeedTrigger : public MSTrigger, public SUMOSAXHandler, public Command {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the lane speed trigger
     * @param[in] net The net the lane speed trigger belongs to
     * @param[in] destLanes List of lanes affected by this speed trigger
     * @param[in] file Name of the file to read the speeds to set from
     */
    MSLaneSpeedTrigger(const std::string& id,
                       const std::vector<MSLane*>& destLanes,
                       const std::string& file);


    /** @brief Destructor */
    virtual ~MSLaneSpeedTrigger();


    /** @brief Executes a switch command
     *
     * Calls processCommand()
     * @param[in] currentTime The current simulation time
     * @see Command
     * @see MSEventControl
     */
    SUMOTime execute(SUMOTime currentTime);


    SUMOTime processCommand(bool move2next, SUMOTime currentTime);

    SUMOReal getDefaultSpeed() const;

    void setOverriding(bool val);

    void setOverridingValue(SUMOReal val);

    SUMOReal getLoadedSpeed();

    /// Returns the current speed
    SUMOReal getCurrentSpeed() const;

protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    virtual void myStartElement(int element,
                                const SUMOSAXAttributes& attrs);

    /** @brief Called on the closing of a tag;
     *
     * @param[in] element ID of the currently closed element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    virtual void myEndElement(int element);
    //@}


protected:
    /** the lane the trigger is responsible for */
    std::vector<MSLane*> myDestLanes;

    /** the speed that will be set on the next call */
    SUMOReal myCurrentSpeed;

    /// The original speed allowed on the lanes
    SUMOReal myDefaultSpeed;

    /// The information whether the read speed shall be overridden
    bool myAmOverriding;

    /// The speed to use if overriding the read speed
    SUMOReal mySpeedOverrideValue;

    std::vector<std::pair<SUMOTime, SUMOReal> > myLoadedSpeeds;
    std::vector<std::pair<SUMOTime, SUMOReal> >::iterator myCurrentEntry;

private:
    /// @brief Invalidated copy constructor.
    MSLaneSpeedTrigger(const MSLaneSpeedTrigger&);

    /// @brief Invalidated assignment operator.
    MSLaneSpeedTrigger& operator=(const MSLaneSpeedTrigger&);

    void init();


private:
    /// The information whether init was called
    bool myDidInit;

};


#endif

/****************************************************************************/

