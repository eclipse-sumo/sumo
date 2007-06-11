/****************************************************************************/
/// @file    SAXWeightsHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 30 Mar 2007
/// @version $Id$
///
// An XML-handler for network weights
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// SAXWeightsHandler::ToRetrieveDefinition methods
// ---------------------------------------------------------------------------
SAXWeightsHandler::ToRetrieveDefinition::ToRetrieveDefinition(const std::string &attributeName,
        bool edgeBased,
        EdgeFloatTimeLineRetriever &destination)
        : myDestination(destination), myAmEdgeBased(edgeBased),
        myMMLAttributeName(attributeName)
{
    myAttributeName = XMLString::transcode(attributeName.c_str());
}


SAXWeightsHandler::ToRetrieveDefinition::~ToRetrieveDefinition()
{
    delete[] myAttributeName;
}


// ---------------------------------------------------------------------------
// SAXWeightsHandler methods
// ---------------------------------------------------------------------------
SAXWeightsHandler::SAXWeightsHandler(const std::vector<ToRetrieveDefinition*> &defs,
                               const std::string &file)
        : SUMOSAXHandler("sumo-netweights", file),
        myCurrentTimeBeg(-1), myCurrentTimeEnd(-1),
        myDefinitions(defs)
{}


SAXWeightsHandler::SAXWeightsHandler(ToRetrieveDefinition *def,
                               const std::string &file)
        : SUMOSAXHandler("sumo-netweights", file),
        myCurrentTimeBeg(-1), myCurrentTimeEnd(-1)
{
    myDefinitions.push_back(def);
}


SAXWeightsHandler::~SAXWeightsHandler() throw()
{
    std::vector<ToRetrieveDefinition*>::iterator i;
    for (i=myDefinitions.begin(); i!=myDefinitions.end(); ++i) {
        delete *i;
    }
}


void SAXWeightsHandler::myStartElement(SumoXMLTag element, const std::string &/*name*/,
                                    const Attributes &attrs) throw()
{
    switch (element) {
    case SUMO_TAG_INTERVAL:
        try {
            myCurrentTimeBeg = getInt(attrs, SUMO_ATTR_BEGIN);
            myCurrentTimeEnd = getInt(attrs, SUMO_ATTR_END);
        } catch (...) {
            MsgHandler::getErrorInstance()->inform("Timestep value is not numeric.");
        }
        break;
    case SUMO_TAG_EDGE:
        myCurrentEdgeID = getStringSecure(attrs, SUMO_ATTR_ID, "");
        tryParse(attrs, true);
        break;
    case SUMO_TAG_LANE:
        tryParse(attrs, false);
        break;
    default:
        break;
    }
}


void
SAXWeightsHandler::tryParse(const Attributes &attrs, bool isEdge)
{
    std::vector<ToRetrieveDefinition*>::iterator i;
    if (isEdge) {
        // process all that want values directly from the edge
        for (i=myDefinitions.begin(); i!=myDefinitions.end(); ++i) {
            if ((*i)->myAmEdgeBased) {
                if (hasAttribute(attrs, (*i)->myAttributeName)) {
                    try {
                        (*i)->myAggValue = getFloat(attrs, (*i)->myAttributeName);
                        (*i)->myNoLanes = 1;
                        (*i)->myHadAttribute = true;
                    } catch (EmptyData &) {
                        MsgHandler::getErrorInstance()->inform("Missing value '" + (*i)->myMMLAttributeName + "' in edge '" + myCurrentEdgeID + "'.");
                    } catch (NumberFormatException &) {
                        MsgHandler::getErrorInstance()->inform("The value should be numeric, but is not ('" + getString(attrs, SUMO_ATTR_TRAVELTIME) + "'\n In edge '" + myCurrentEdgeID + "' at time step " + toString<long>(myCurrentTimeBeg) + ".");
                    }
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
                    (*i)->myAggValue += getFloat(attrs, (*i)->myAttributeName);
                    ++((*i)->myNoLanes);
                    (*i)->myHadAttribute = true;
                } catch (EmptyData &) {
                    MsgHandler::getErrorInstance()->inform("Missing value '" + (*i)->myMMLAttributeName + "' in edge '" + myCurrentEdgeID + "'.");
                } catch (NumberFormatException &) {
                    MsgHandler::getErrorInstance()->inform("The value should be numeric, but is not ('" + getString(attrs, SUMO_ATTR_TRAVELTIME) + "'\n In edge '" + myCurrentEdgeID + "' at time step " + toString<long>(myCurrentTimeBeg) + ".");
                }
            }
        }
    }
}


void
SAXWeightsHandler::myCharacters(SumoXMLTag /*element*/, const std::string &/*name*/,
                             const std::string &/*chars*/) throw()
{}


void
SAXWeightsHandler::myEndElement(SumoXMLTag element, const std::string &/*name*/) throw()
{
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

