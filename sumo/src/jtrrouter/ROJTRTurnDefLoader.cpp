/****************************************************************************/
/// @file    ROJTRTurnDefLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Loader for the of turning percentages and source/sink definitions
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

#include <set>
#include <string>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
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
ROJTRTurnDefLoader::ROJTRTurnDefLoader(RONet &net) throw()
        : SUMOSAXHandler("turn-definitions"), myNet(net),
        myIntervalBegin(0), myIntervalEnd(86400), myEdge(0),
        myHaveWarnedAboutDeprecatedSources(false),
        myHaveWarnedAboutDeprecatedSinks(false) {}


ROJTRTurnDefLoader::~ROJTRTurnDefLoader() throw() {}


void
ROJTRTurnDefLoader::myStartElement(SumoXMLTag element,
                                   const SUMOSAXAttributes &attrs) throw(ProcessError) {
    bool ok = true;
    switch (element) {
    case SUMO_TAG_INTERVAL:
        myIntervalBegin = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, "interval", 0, ok);
        myIntervalEnd = attrs.getSUMOTimeReporting(SUMO_ATTR_END, "interval", 0, ok);
        break;
    case SUMO_TAG_FROMEDGE:
        beginFromEdge(attrs);
        break;
    case SUMO_TAG_TOEDGE:
        addToEdge(attrs);
        break;
    case SUMO_TAG_SINK:
        if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
            std::string edges = attrs.getStringReporting(SUMO_ATTR_EDGES, "sink", 0, ok);
            StringTokenizer st(edges, StringTokenizer::WHITECHARS);
            while (st.hasNext()) {
                std::string id = st.next();
                ROEdge *edge = myNet.getEdge(id);
                if (edge==0) {
                    throw ProcessError("The edge '" + id + "' declared as a sink is not known.");
                }
                edge->setType(ROEdge::ET_SINK);
            }
        }
        break;
    case SUMO_TAG_SOURCE:
        if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
            std::string edges = attrs.getStringReporting(SUMO_ATTR_EDGES, "source", 0, ok);
            StringTokenizer st(edges, StringTokenizer::WHITECHARS);
            while (st.hasNext()) {
                std::string id = st.next();
                ROEdge *edge = myNet.getEdge(id);
                if (edge==0) {
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
ROJTRTurnDefLoader::myCharacters(SumoXMLTag element,
                                 const std::string &chars) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_SINK: {
        ROEdge *edge = myNet.getEdge(chars);
        if (edge==0) {
            throw ProcessError("The edge '" + chars + "' declared as a sink is not known.");
        }
        if (!myHaveWarnedAboutDeprecatedSinks) {
            myHaveWarnedAboutDeprecatedSinks = true;
            MsgHandler::getWarningInstance()->inform("Using characters for sinks is deprecated; use attribute 'edges' instead.");
        }
        edge->setType(ROEdge::ET_SINK);
    }
    break;
    case SUMO_TAG_SOURCE: {
        ROEdge *edge = myNet.getEdge(chars);
        if (edge==0) {
            throw ProcessError("The edge '" + chars + "' declared as a source is not known.");
        }
        if (!myHaveWarnedAboutDeprecatedSources) {
            myHaveWarnedAboutDeprecatedSources = true;
            MsgHandler::getWarningInstance()->inform("Using characters for sources is deprecated; use attribute 'edges' instead.");
        }
        edge->setType(ROEdge::ET_SOURCE);
    }
    break;
    default:
        break;
    }
}


void
ROJTRTurnDefLoader::beginFromEdge(const SUMOSAXAttributes &attrs) throw() {
    myEdge = 0;
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("from-edge", id)) {
        return;
    }
    //
    myEdge = static_cast<ROJTREdge*>(myNet.getEdge(id));
    if (myEdge==0) {
        MsgHandler::getErrorInstance()->inform("The edge '" + id + "' is not known within the network (within a 'from-edge' tag).");
        return;
    }
}


void
ROJTRTurnDefLoader::addToEdge(const SUMOSAXAttributes &attrs) throw() {
    if (myEdge==0) {
        return;
    }
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("to-edge", id)) {
        return;
    }
    //
    ROJTREdge *edge = static_cast<ROJTREdge*>(myNet.getEdge(id));
    if (edge==0) {
        MsgHandler::getErrorInstance()->inform("The edge '" + id + "' is not known within the network (within a 'to-edge' tag).");
        return;
    }
    bool ok = true;
    SUMOReal probability = attrs.getSUMORealReporting(SUMO_ATTR_PROB, "to-edge", id.c_str(), ok);
    if (ok) {
        if (probability<0) {
            MsgHandler::getErrorInstance()->inform("'probability' must be positive (in definition of to-edge '" + id + "').");
        } else {
            myEdge->addFollowerProbability(edge, myIntervalBegin, myIntervalEnd, probability);
        }
    }
}



/****************************************************************************/

