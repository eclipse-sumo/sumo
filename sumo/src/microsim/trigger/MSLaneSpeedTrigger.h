/****************************************************************************/
/// @file    MSLaneSpeedTrigger.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Changes the speed allowed on a set of lanes
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
class MSLaneSpeedTrigger : public MSTrigger, public SUMOSAXHandler
{
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the lane speed trigger
     * @param[in] net The net the lane speed trigger belongs to
     * @param[in] destLanes List of lanes affected by this speed trigger
     * @param[in] file Name of the file to read the speeds to set from
     */
    MSLaneSpeedTrigger(const std::string &id,
                       const std::vector<MSLane*> &destLanes,
                       const std::string &file) throw(ProcessError);

    /** destructor */
    virtual ~MSLaneSpeedTrigger() throw();

    SUMOTime execute(SUMOTime currentTime) throw(ProcessError);

    SUMOTime processCommand(bool move2next);

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
    virtual void myStartElement(SumoXMLTag element,
                                const SUMOSAXAttributes &attrs) throw(ProcessError);

    /** @brief Called on the closing of a tag;
     *
     * @param[in] element ID of the currently closed element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    virtual void myEndElement(SumoXMLTag element) throw(ProcessError);
    //@}


protected:
    /// Define the container for those lanes that shall be manipulated
    typedef std::vector<MSLane*> LaneCont;

    /** the lane the trigger is responsible for */
    LaneCont myDestLanes;

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

    void init() throw(ProcessError);


private:
    /// The information whether init was called
    bool myDidInit;

};


#endif

/****************************************************************************/

