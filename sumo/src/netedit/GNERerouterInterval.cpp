/****************************************************************************/
/// @file    GNERerouterInterval.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id: GNERerouter.cpp 22699 2017-01-25 14:56:03Z behrisch $
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
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

#include <foreign/polyfonts/polyfonts.h>
#include <iostream>
#include <string>
#include <utility>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEChange_Attribute.h"
#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNEDestProbReroute.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNERerouterDialog.h"
#include "GNERerouterInterval.h"
#include "GNERouteProbReroute.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"
#include "GNEViewNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterInterval::GNERerouterInterval(SUMOReal begin, SUMOReal end) :
    std::pair<SUMOReal, SUMOReal>(begin, end) {
}


GNERerouterInterval::~GNERerouterInterval() {
}


void
GNERerouterInterval::insertClosingReroutes(GNEClosingReroute* cr) {
    // Check if was already inserted
    for (std::vector<GNEClosingReroute*>::iterator i = myClosingReroutes.begin(); i != myClosingReroutes.end(); i++) {
        if ((*i) == cr) {
            throw ProcessError("closing reroute " + cr->getClosedEdgeId() + " already inserted");
        }
    }
    // insert in vector
    myClosingReroutes.push_back(cr);
}


void
GNERerouterInterval::removeClosingReroutes(GNEClosingReroute* cr) {
    // find and remove
    for (std::vector<GNEClosingReroute*>::iterator i = myClosingReroutes.begin(); i != myClosingReroutes.end(); i++) {
        if ((*i) == cr) {
            myClosingReroutes.erase(i);
            return;
        }
    }
    // Throw error if don't exist
    throw ProcessError("closing reroute " + cr->getClosedEdgeId() + " not exist");
}


void
GNERerouterInterval::insertDestProbReroutes(GNEDestProbReroute* dpr) {
    // Check if was already inserted
    for (std::vector<GNEDestProbReroute*>::iterator i = myDestProbReroutes.begin(); i != myDestProbReroutes.end(); i++) {
        if ((*i) == dpr) {
            throw ProcessError("destiny probability reroute " + dpr->getNewDestinationId() + " already inserted");
        }
    }
    // insert in vector
    myDestProbReroutes.push_back(dpr);
}


void
GNERerouterInterval::removeDestProbReroutes(GNEDestProbReroute* dpr) {
    // find and remove
    for (std::vector<GNEDestProbReroute*>::iterator i = myDestProbReroutes.begin(); i != myDestProbReroutes.end(); i++) {
        if ((*i) == dpr) {
            myDestProbReroutes.erase(i);
            return;
        }
    }
    // Throw error if don't exist
    throw ProcessError("destiny probability reroute " + dpr->getNewDestinationId() + " not exist");
}


void
GNERerouterInterval::insertRouteProbReroute(GNERouteProbReroute* rpr) {
    // Check if was already inserted
    for (std::vector<GNERouteProbReroute*>::iterator i = myRouteProbReroutes.begin(); i != myRouteProbReroutes.end(); i++) {
        if ((*i) == rpr) {
            throw ProcessError("route probability reroute " + rpr->getNewRouteId() + " already inserted");
        }
    }
    // insert in vector
    myRouteProbReroutes.push_back(rpr);
}


void
GNERerouterInterval::removeRouteProbReroute(GNERouteProbReroute* rpr) {
    // find and remove
    for (std::vector<GNERouteProbReroute*>::iterator i = myRouteProbReroutes.begin(); i != myRouteProbReroutes.end(); i++) {
        if ((*i) == rpr) {
            myRouteProbReroutes.erase(i);
            return;
        }
    }
    // Throw error if don't exist
    throw ProcessError("route probability reroute " + rpr->getNewRouteId() + " not exist");
}


SUMOReal
GNERerouterInterval::getBegin() const {
    return first;
}


SUMOReal
GNERerouterInterval::getEnd() const {
    return second;
}


std::vector<GNEClosingReroute*>
GNERerouterInterval::getClosingReroutes() const {
    return myClosingReroutes;
}


std::vector<GNEDestProbReroute*>
GNERerouterInterval::getDestProbReroutes() const {
    return myDestProbReroutes;
}


std::vector<GNERouteProbReroute*>
GNERerouterInterval::getRouteProbReroutes() const {
    return myRouteProbReroutes;
}


/****************************************************************************/
