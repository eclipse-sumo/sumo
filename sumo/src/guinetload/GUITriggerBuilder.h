/****************************************************************************/
/// @file    GUITriggerBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 26.04.2004
/// @version $Id$
///
// A building helper for triggers
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
#ifndef GUITriggerBuilder_h
#define GUITriggerBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <netload/NLTriggerBuilder.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSTrigger;
class MSNet;
class MSLaneSpeedTrigger;
class MSTriggerControl;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUITriggerBuilder
 * This class builds trigger objects in their non-gui version
 */
class GUITriggerBuilder : public NLTriggerBuilder
{
public:
    /// Constructor
    GUITriggerBuilder();

    /// Destructor
    ~GUITriggerBuilder();

protected:
    /// builds a lane speed trigger
    virtual MSLaneSpeedTrigger *buildLaneSpeedTrigger(MSNet &net,
            const std::string &id, const std::vector<MSLane*> &destLanes,
            const std::string &file);

    /// builds an emitter
    virtual MSEmitter *buildLaneEmitTrigger(MSNet &net,
                                            const std::string &id, MSLane *destLane, SUMOReal pos,
                                            const std::string &file);

    /// builds an emitter
    virtual MSTriggeredRerouter *buildRerouter(MSNet &net,
            const std::string &id, std::vector<MSEdge*> &edges,
            SUMOReal prob, const std::string &file);

    /// builds a vehicle actor
    virtual MSE1VehicleActor *buildVehicleActor(MSNet &, const std::string &id,
            MSLane *lane, SUMOReal pos, unsigned int la,
            unsigned int cell, unsigned int type);

    /// builds a bus stop
    virtual MSBusStop* buildBusStop(MSNet &net,
                                    const std::string &id, const std::vector<std::string> &lines,
                                    MSLane *lane, SUMOReal frompos, SUMOReal topos);

};


#endif

/****************************************************************************/

