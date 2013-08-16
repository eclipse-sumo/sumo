/****************************************************************************/
/// @file    ROJTRTurnDefLoader.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Loader for the of turning percentages and source/sink definitions
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <set>
#include <string>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <router/RONet.h>
#include "ROJTREdge.h"
#include "ROJTRTurnDefLoader.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROJTRTurnDefLoader::ROJTRTurnDefLoader(RONet& net)
    : SUMOSAXHandler("turn-ratio-file"), myNet(net),
      myIntervalBegin(0), myIntervalEnd(SUMOTime_MAX), myEdge(0) {}


ROJTRTurnDefLoader::~ROJTRTurnDefLoader() {}


void
ROJTRTurnDefLoader::myStartElement(int element,
                                   const SUMOSAXAttributes& attrs) {
    bool ok = true;
    switch (element) {
        case SUMO_TAG_INTERVAL:
            myIntervalBegin = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, 0, ok);
            myIntervalEnd = attrs.getSUMOTimeReporting(SUMO_ATTR_END, 0, ok);
            break;
        case SUMO_TAG_FROMEDGE:
            beginFromEdge(attrs);
            break;
        case SUMO_TAG_TOEDGE:
            addToEdge(attrs);
            break;
        case SUMO_TAG_SINK:
            if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
                std::string edges = attrs.get<std::string>(SUMO_ATTR_EDGES, 0, ok);
                StringTokenizer st(edges, StringTokenizer::WHITECHARS);
                while (st.hasNext()) {
                    std::string id = st.next();
                    ROEdge* edge = myNet.getEdge(id);
                    if (edge == 0) {
                        throw ProcessError("The edge '" + id + "' declared as a sink is not known.");
                    }
                    edge->setType(ROEdge::ET_SINK);
                }
            }
            break;
        case SUMO_TAG_SOURCE:
            if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
                std::string edges = attrs.get<std::string>(SUMO_ATTR_EDGES, 0, ok);
                StringTokenizer st(edges, StringTokenizer::WHITECHARS);
                while (st.hasNext()) {
                    std::string id = st.next();
                    ROEdge* edge = myNet.getEdge(id);
                    if (edge == 0) {
                        throw ProcessError("The edge '" + id + "' declared as a source is not known.");
                    }
                    edge->setType(ROEdge::ET_SOURCE);
                }
            }
            break;
        default:
            break;
    }
}


void
ROJTRTurnDefLoader::beginFromEdge(const SUMOSAXAttributes& attrs) {
    myEdge = 0;
    bool ok = true;
    // get the id, report an error if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    //
    myEdge = static_cast<ROJTREdge*>(myNet.getEdge(id));
    if (myEdge == 0) {
        WRITE_ERROR("The edge '" + id + "' is not known within the network (within a 'from-edge' tag).");
        return;
    }
}


void
ROJTRTurnDefLoader::addToEdge(const SUMOSAXAttributes& attrs) {
    if (myEdge == 0) {
        return;
    }
    bool ok = true;
    // get the id, report an error if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    //
    ROJTREdge* edge = static_cast<ROJTREdge*>(myNet.getEdge(id));
    if (edge == 0) {
        WRITE_ERROR("The edge '" + id + "' is not known within the network (within a 'to-edge' tag).");
        return;
    }
    SUMOReal probability = attrs.get<SUMOReal>(SUMO_ATTR_PROB, id.c_str(), ok);
    if (ok) {
        if (probability < 0) {
            WRITE_ERROR("'probability' must be positive (in definition of to-edge '" + id + "').");
        } else {
            myEdge->addFollowerProbability(edge, myIntervalBegin, myIntervalEnd, probability);
        }
    }
}



/****************************************************************************/

