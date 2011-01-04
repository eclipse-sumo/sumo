/****************************************************************************/
/// @file    GUITriggerBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 26.04.2004
/// @version $Id$
///
// Builds trigger objects for guisim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <guisim/GUILaneSpeedTrigger.h>
#include <guisim/GUINet.h>
#include <guisim/GUIEmitter.h>
#include <guisim/GUITriggeredRerouter.h>
#include <guisim/GUIBusStop.h>
#include "GUITriggerBuilder.h"
#include <utils/gui/globjects/GUIGlObjectStorage.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUITriggerBuilder::GUITriggerBuilder() throw() {}


GUITriggerBuilder::~GUITriggerBuilder() throw() {}


MSLaneSpeedTrigger*
GUITriggerBuilder::buildLaneSpeedTrigger(MSNet &net,
        const std::string &id, const std::vector<MSLane*> &destLanes,
        const std::string &file) throw(ProcessError) {
    GUILaneSpeedTrigger *lst = new GUILaneSpeedTrigger(GUIGlObjectStorage::gIDStorage, id, destLanes, file);
    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(lst);
    return lst;
}


void
GUITriggerBuilder::buildLaneEmitTrigger(MSNet &net,
                                        const std::string &id,
                                        MSLane *destLane,
                                        SUMOReal pos,
                                        const std::string &file) throw() {
    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(new GUIEmitter(GUIGlObjectStorage::gIDStorage, id, net, destLane, pos, file));
}


void
GUITriggerBuilder::buildRerouter(MSNet &net, const std::string &id,
                                 std::vector<MSEdge*> &edges,
                                 SUMOReal prob, const std::string &file, bool off) throw() {
    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(new GUITriggeredRerouter(GUIGlObjectStorage::gIDStorage, id, edges, prob, file, off));
}


void
GUITriggerBuilder::buildBusStop(MSNet &net, const std::string &id,
                                const std::vector<std::string> &lines,
                                MSLane *lane,
                                SUMOReal frompos, SUMOReal topos) throw() {
    GUIBusStop *stop = new GUIBusStop(GUIGlObjectStorage::gIDStorage, id, lines, *lane, frompos, topos);
    net.addBusStop(stop);
    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(stop);
}


/****************************************************************************/

