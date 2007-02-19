/****************************************************************************/
/// @file    MSLaneSpeedTrigger.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Class that realises the setting of a lane's maximum speed triggered by
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
#ifndef MSLaneSpeedTrigger_h
#define MSLaneSpeedTrigger_h
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

#include <string>
#include <vector>
#include <utils/helpers/Command.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include "MSTrigger.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * MSLaneSpeedTrigger
 * This trigger reads the next maximum velocity of the lane he is
 * responsible for from a file and sets it.
 * Lanes with variable speeds are so possible.
 */
class MSLaneSpeedTrigger : public MSTrigger, public SUMOSAXHandler
{
public:
    /** constructor */
    MSLaneSpeedTrigger(const std::string &id,
                       MSNet &net, const std::vector<MSLane*> &destLanes,
                       const std::string &aXMLFilename);

    /** destructor */
    virtual ~MSLaneSpeedTrigger();

    SUMOTime execute(SUMOTime currentTime);

    SUMOTime processCommand(bool move2next);

    SUMOReal getDefaultSpeed() const;

    void setOverriding(bool val);

    void setOverridingValue(SUMOReal val);

    SUMOReal getLoadedSpeed();

    /// Returns the current speed
    SUMOReal getCurrentSpeed() const;

protected:
    /** the implementation of the SAX-handler interface for reading
        element begins */
    virtual void myStartElement(int element, const std::string &name,
                                const Attributes &attrs);

    /** the implementation of the SAX-handler interface for reading
        characters */
    void myCharacters(int element, const std::string &name,
                      const std::string &chars);

    /** the implementation of the SAX-handler interface for reading
        element ends */
    void myEndElement(int element, const std::string &name);


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

};


#endif

/****************************************************************************/

