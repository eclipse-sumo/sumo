/****************************************************************************/
/// @file    SAXWeightsHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 30 Mar 2007
/// @version $Id$
///
// An XML-handler for network weights
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
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include "SAXWeightsHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// SAXWeightsHandler::ToRetrieveDefinition methods
// ---------------------------------------------------------------------------
SAXWeightsHandler::ToRetrieveDefinition::ToRetrieveDefinition(const std::string &attributeName,
        bool edgeBased, EdgeFloatTimeLineRetriever &destination)
        : myAttributeName(attributeName), myAmEdgeBased(edgeBased), myDestination(destination) {
}


SAXWeightsHandler::ToRetrieveDefinition::~ToRetrieveDefinition() {
}


// ---------------------------------------------------------------------------
// SAXWeightsHandler methods
// ---------------------------------------------------------------------------
SAXWeightsHandler::SAXWeightsHandler(const std::vector<ToRetrieveDefinition*> &defs,
                                     const std::string &file)
        : SUMOSAXHandler(file), myDefinitions(defs),
        myCurrentTimeBeg(-1), myCurrentTimeEnd(-1) {}


SAXWeightsHandler::SAXWeightsHandler(ToRetrieveDefinition *def,
                                     const std::string &file)
        : SUMOSAXHandler(file),
        myCurrentTimeBeg(-1), myCurrentTimeEnd(-1) {
    myDefinitions.push_back(def);
}


SAXWeightsHandler::~SAXWeightsHandler() throw() {
    std::vector<ToRetrieveDefinition*>::iterator i;
    for (i=myDefinitions.begin(); i!=myDefinitions.end(); ++i) {
        delete *i;
    }
}


void SAXWeightsHandler::myStartElement(SumoXMLTag element,
                                       const SUMOSAXAttributes &attrs) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_INTERVAL: {
        bool ok = true;
        myCurrentTimeBeg = attrs.getSUMORealReporting(SUMO_ATTR_BEGIN, "weights/interval", 0, ok);
        myCurrentTimeEnd = attrs.getSUMORealReporting(SUMO_ATTR_END, "weights/interval", 0, ok);
    }
    break;
    case SUMO_TAG_EDGE: {
        bool ok = true;
        myCurrentEdgeID = attrs.getOptStringReporting(SUMO_ATTR_ID, 0, 0, ok, "");
        tryParse(attrs, true);
    }
    break;
    case SUMO_TAG_LANE: {
        tryParse(attrs, false);
    }
    break;
    default:
        break;
    }
}


void
SAXWeightsHandler::tryParse(const SUMOSAXAttributes &attrs, bool isEdge) {
    // !!!! no error handling!
    std::vector<ToRetrieveDefinition*>::iterator i;
    if (isEdge) {
        // process all that want values directly from the edge
        for (i=myDefinitions.begin(); i!=myDefinitions.end(); ++i) {
            if ((*i)->myAmEdgeBased) {
                if (attrs.hasAttribute((*i)->myAttributeName)) {
                    (*i)->myAggValue = attrs.getFloat((*i)->myAttributeName);
                    (*i)->myNoLanes = 1;
                    (*i)->myHadAttribute = true;
                } else {
                    (*i)->myHadAttribute = false;
                }
            } else {
                (*i)->myAggValue = 0;
                (*i)->myNoLanes = 0;
            }
        }
    } else {
        // process the current lane values
        for (i=myDefinitions.begin(); i!=myDefinitions.end(); ++i) {
            if (!(*i)->myAmEdgeBased) {
                try {
                    (*i)->myAggValue += attrs.getFloat((*i)->myAttributeName);
                    ++((*i)->myNoLanes);
                    (*i)->myHadAttribute = true;
                } catch (EmptyData &) {
                    MsgHandler::getErrorInstance()->inform("Missing value '" + (*i)->myAttributeName + "' in edge '" + myCurrentEdgeID + "'.");
                } catch (NumberFormatException &) {
                    MsgHandler::getErrorInstance()->inform("The value should be numeric, but is not.\n In edge '" + myCurrentEdgeID + "' at time step " + toString(myCurrentTimeBeg) + ".");
                }
            }
        }
    }
}


void
SAXWeightsHandler::myEndElement(SumoXMLTag element) throw() {
    if (element==SUMO_TAG_EDGE) {
        std::vector<ToRetrieveDefinition*>::iterator i;
        for (i=myDefinitions.begin(); i!=myDefinitions.end(); ++i) {
            if ((*i)->myHadAttribute) {
                (*i)->myDestination.addEdgeWeight(myCurrentEdgeID,
                                                  (*i)->myAggValue/(SUMOReal)(*i)->myNoLanes,
                                                  myCurrentTimeBeg, myCurrentTimeEnd);
            }
        }
    }
}



/****************************************************************************/

