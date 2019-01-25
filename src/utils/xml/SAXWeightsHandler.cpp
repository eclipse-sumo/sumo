/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SAXWeightsHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Fri, 30 Mar 2007
/// @version $Id$
///
// An XML-handler for network weights
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include "SAXWeightsHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// SAXWeightsHandler::ToRetrieveDefinition methods
// ---------------------------------------------------------------------------
SAXWeightsHandler::ToRetrieveDefinition::ToRetrieveDefinition(const std::string& attributeName,
        bool edgeBased, EdgeFloatTimeLineRetriever& destination)
    : myAttributeName(attributeName), myAmEdgeBased(edgeBased), myDestination(destination) {
}


SAXWeightsHandler::ToRetrieveDefinition::~ToRetrieveDefinition() {
}


// ---------------------------------------------------------------------------
// SAXWeightsHandler methods
// ---------------------------------------------------------------------------
SAXWeightsHandler::SAXWeightsHandler(const std::vector<ToRetrieveDefinition*>& defs,
                                     const std::string& file)
    : SUMOSAXHandler(file), myDefinitions(defs),
      myCurrentTimeBeg(-1), myCurrentTimeEnd(-1) {}


SAXWeightsHandler::SAXWeightsHandler(ToRetrieveDefinition* def,
                                     const std::string& file)
    : SUMOSAXHandler(file),
      myCurrentTimeBeg(-1), myCurrentTimeEnd(-1) {
    myDefinitions.push_back(def);
}


SAXWeightsHandler::~SAXWeightsHandler() {
    std::vector<ToRetrieveDefinition*>::iterator i;
    for (i = myDefinitions.begin(); i != myDefinitions.end(); ++i) {
        delete *i;
    }
}


void SAXWeightsHandler::myStartElement(int element,
                                       const SUMOSAXAttributes& attrs) {
    switch (element) {
        case SUMO_TAG_INTERVAL: {
            bool ok = true;
            myCurrentTimeBeg = STEPS2TIME(attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, nullptr, ok));
            myCurrentTimeEnd = STEPS2TIME(attrs.getSUMOTimeReporting(SUMO_ATTR_END, nullptr, ok));
        }
        break;
        case SUMO_TAG_EDGE: {
            bool ok = true;
            myCurrentEdgeID = attrs.getOpt<std::string>(SUMO_ATTR_ID, nullptr, ok, "");
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
SAXWeightsHandler::tryParse(const SUMOSAXAttributes& attrs, bool isEdge) {
    // !!!! no error handling!
    std::vector<ToRetrieveDefinition*>::iterator i;
    if (isEdge) {
        // process all that want values directly from the edge
        for (i = myDefinitions.begin(); i != myDefinitions.end(); ++i) {
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
        for (i = myDefinitions.begin(); i != myDefinitions.end(); ++i) {
            if (!(*i)->myAmEdgeBased) {
                try {
                    (*i)->myAggValue += attrs.getFloat((*i)->myAttributeName);
                    ++((*i)->myNoLanes);
                    (*i)->myHadAttribute = true;
                } catch (EmptyData&) {
                    WRITE_ERROR("Missing value '" + (*i)->myAttributeName + "' in edge '" + myCurrentEdgeID + "'.");
                } catch (NumberFormatException&) {
                    WRITE_ERROR("The value should be numeric, but is not.\n In edge '" + myCurrentEdgeID + "' at time step " + toString(myCurrentTimeBeg) + ".");
                }
            }
        }
    }
}


void
SAXWeightsHandler::myEndElement(int element) {
    if (element == SUMO_TAG_EDGE) {
        std::vector<ToRetrieveDefinition*>::iterator i;
        for (i = myDefinitions.begin(); i != myDefinitions.end(); ++i) {
            if ((*i)->myHadAttribute) {
                (*i)->myDestination.addEdgeWeight(myCurrentEdgeID,
                                                  (*i)->myAggValue / (double)(*i)->myNoLanes,
                                                  myCurrentTimeBeg, myCurrentTimeEnd);
            }
        }
    }
}



/****************************************************************************/

