/****************************************************************************/
/// @file    GUITriggerBuilder.cpp
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <fstream>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <microsim/trigger/MSTrigger.h>
#include <guisim/GUILaneSpeedTrigger.h>
#include <guisim/GUIEmitter.h>
#include <guisim/GUITriggeredRerouter.h>
#include <guisim/GUIE1VehicleActor.h>
#include <guisim/GUIBusStop.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include "GUITriggerBuilder.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
GUITriggerBuilder::GUITriggerBuilder()
{}


GUITriggerBuilder::~GUITriggerBuilder()
{}


MSLaneSpeedTrigger *
GUITriggerBuilder::buildLaneSpeedTrigger(MSNet &net,
        const std::string &id, const std::vector<MSLane*> &destLanes,
        const std::string &file)
{
    return new GUILaneSpeedTrigger(id, net, destLanes, file);
}




MSEmitter *
GUITriggerBuilder::buildLaneEmitTrigger(MSNet &net,
                                        const std::string &id,
                                        MSLane *destLane,
                                        SUMOReal pos,
                                        const std::string &file)
{
    return new GUIEmitter(id, net, destLane, pos, file);
}



MSTriggeredRerouter *
GUITriggerBuilder::buildRerouter(MSNet &, const std::string &id,
                                 std::vector<MSEdge*> &edges,
                                 SUMOReal prob, const std::string &file)
{
    return new GUITriggeredRerouter(id, edges, prob, file);
}


MSE1VehicleActor *
GUITriggerBuilder::buildVehicleActor(MSNet &, const std::string &id,
                                     MSLane *lane, SUMOReal pos, unsigned int la,
                                     unsigned int cell, unsigned int type)
{
    return new GUIE1VehicleActor(id, lane, pos, la, cell, type);
}


MSBusStop*
GUITriggerBuilder::buildBusStop(MSNet &net, const std::string &id,
                                const std::vector<std::string> &lines,
                                MSLane *lane,
                                SUMOReal frompos, SUMOReal topos)
{
    return new GUIBusStop(id, net, lines, *lane, frompos, topos);
}



/****************************************************************************/

