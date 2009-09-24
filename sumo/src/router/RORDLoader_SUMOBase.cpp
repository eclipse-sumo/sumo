/****************************************************************************/
/// @file    RORDLoader_SUMOBase.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The base class for SUMO-native route handlers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include "RORDLoader_SUMOBase.h"
#include <utils/common/SUMOVTypeParameter.h>
#include "RORouteDef.h"
#include "RONet.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include "ROVehicle.h"
#include "RORouteDef_Alternatives.h"
#include "RORouteDef_Complete.h"
#include "RORoute.h"
#include <utils/xml/SUMOVehicleParserHelper.h>

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
RORDLoader_SUMOBase::RORDLoader_SUMOBase(RONet &net,
        SUMOTime begin, SUMOTime end,
        SUMOReal gawronBeta, SUMOReal gawronA,
        int maxRouteNumber, bool tryRepair,
        const std::string &file) throw(ProcessError)
        : ROTypedXMLRoutesLoader(net, begin, end, file),
        myVehicleParameter(0), myCurrentIsOk(true), myAltIsValid(true), myHaveNextRoute(false),
        myCurrentAlternatives(0),
        myGawronBeta(gawronBeta), myGawronA(gawronA), myMaxRouteNumber(maxRouteNumber),
        myCurrentRoute(0), myCurrentDepart(-1), myTryRepair(tryRepair), myColor(0),
        myCurrentVType(0)
{
}


RORDLoader_SUMOBase::~RORDLoader_SUMOBase() throw() {
    // clean up (on failure)
    delete myCurrentAlternatives;
    delete myCurrentRoute;
    delete myVehicleParameter;
    delete myColor;
}


void
RORDLoader_SUMOBase::myStartElement(SumoXMLTag element,
                                    const SUMOSAXAttributes &attrs) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_ROUTE:
        startRoute(attrs);
        break;
    case SUMO_TAG_VEHICLE:
        // try to parse the vehicle definition
        delete myVehicleParameter;
        myVehicleParameter = 0;
        myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs);
        if (myVehicleParameter!=0) {
            myCurrentDepart = myVehicleParameter->depart;
        }
        myCurrentIsOk = myVehicleParameter!=0;
        break;
    case SUMO_TAG_VTYPE:
        myCurrentVType = SUMOVehicleParserHelper::beginVTypeParsing(attrs);
        break;
    case SUMO_TAG_ROUTE_DISTRIBUTION:
        myAltIsValid = true;
        startAlternative(attrs);
        if (!myCurrentIsOk) {
            myAltIsValid = false;
        }
        break;
    default:
        break;
    }
    // parse embedded vtype information
    if(myCurrentVType!=0&&element!=SUMO_TAG_VTYPE) {
        SUMOVehicleParserHelper::parseVTypeEmbedded(*myCurrentVType, element, attrs);
        return;
    }
    if (!myCurrentIsOk) {
        throw ProcessError();
    }
}


void
RORDLoader_SUMOBase::startRoute(const SUMOSAXAttributes &attrs) {
    delete myColor;
    myColor = 0;
    if (!myAltIsValid) {
        return;
    }
    if (attrs.hasAttribute(SUMO_ATTR_COLOR)) {
        myColor = new RGBColor(RGBColor::parseColor(attrs.getString(SUMO_ATTR_COLOR)));
    }
    if (myCurrentAlternatives==0) {
        myCurrentIsOk = true;
        // parse plain route...
        try {
            if (myVehicleParameter!=0) {
                myCurrentRouteName = attrs.getStringSecure(SUMO_ATTR_ID, "!" + myVehicleParameter->id);
            } else {
                myCurrentRouteName = attrs.getString(SUMO_ATTR_ID);
            }
        } catch (EmptyData &) {
            myCurrentRouteName = "";
            MsgHandler::getErrorInstance()->inform("Missing id in route.");
            myCurrentIsOk = false;
        }
    } else {
        // parse route alternative...
        myCost = attrs.getSUMORealReporting(SUMO_ATTR_COST, "route(alternative)", myCurrentAlternatives->getID().c_str(), myCurrentIsOk);
        myProbability = attrs.getSUMORealReporting(SUMO_ATTR_PROB, "route(alternative)", myCurrentAlternatives->getID().c_str(), myCurrentIsOk);
        if (myCurrentIsOk&&myCost<0) {
            MsgHandler::getErrorInstance()->inform("Invalid cost in alternative for route '" + myCurrentAlternatives->getID() + "' (" + toString<SUMOReal>(myCost) + ").");
            myCurrentIsOk = false;
            return;
        }
        if (myCurrentIsOk&&myProbability<0) {
            MsgHandler::getErrorInstance()->inform("Invalid probability in alternative for route '" + myCurrentAlternatives->getID() + "' (" + toString<SUMOReal>(myProbability) + ").");
            myCurrentIsOk = false;
            return;
        }
    }
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        myCharacters(SUMO_TAG_ROUTE, attrs.getString(SUMO_ATTR_EDGES));
    }
}


void
RORDLoader_SUMOBase::startAlternative(const SUMOSAXAttributes &attrs) {
    // try to get the id
    myCurrentIsOk = true;
    string id;
    if (myVehicleParameter!=0) {
        id = myVehicleParameter->id;
        if (id=="") {
            MsgHandler::getErrorInstance()->inform("Missing 'id' of a routeDistribution.");
            myCurrentIsOk = false;
            return;
        }
        id = "!" + id;
    } else {
        if (!attrs.setIDFromAttributes("routeDistribution", id)) {
            myCurrentIsOk = false;
            return;
        }
    }
    // try to get the index of the last element
    int index = attrs.getIntReporting(SUMO_ATTR_LAST, "route", id.c_str(), myCurrentIsOk);
    if (myCurrentIsOk&&index<0) {
        MsgHandler::getErrorInstance()->inform("Negative index of a route alternative (id='" + id + "').");
        myCurrentIsOk = false;
        return;
    }
    // build the alternative cont
    myCurrentAlternatives = new RORouteDef_Alternatives(id, index, myGawronBeta, myGawronA, myMaxRouteNumber);
}

void
RORDLoader_SUMOBase::myCharacters(SumoXMLTag element,
                                  const std::string &chars) throw(ProcessError) {
    // process routes only, all other elements do
    //  not have embedded characters
    if (element!=SUMO_TAG_ROUTE) {
        return;
    }
    if (!myAltIsValid) {
        return;
    }
    if (myCurrentRoute!=0) {
        return;
    }
    // check whether the costs and the probability are valid
    if (myCurrentAlternatives!=0) {
        if (myCost<0||myProbability<0||!myCurrentIsOk) {
            return;
        }
    }
    // build the list of edges
    std::vector<const ROEdge*> *list = new std::vector<const ROEdge*>();
    StringTokenizer st(chars);
    while (myCurrentIsOk&&st.hasNext()) { // !!! too slow !!!
        string id = st.next();
        ROEdge *edge = myNet.getEdge(id);
        if (edge!=0) {
            list->push_back(edge);
        } else {
            if (!myTryRepair) {
                std::string rid = myCurrentAlternatives!=0 ? myCurrentAlternatives->getID() : myCurrentRouteName;
                MsgHandler::getErrorInstance()->inform("The route '" + rid + "' contains the unknown edge '" + id + "'.");
                myCurrentIsOk = false;
            }
        }
    }
    if (myCurrentIsOk) {
        if (myCurrentAlternatives!=0) {
            myCurrentAlternatives->addLoadedAlternative(
                new RORoute(myCurrentAlternatives->getID(), myCost, myProbability, *list, myColor));
        } else {
            myCurrentRoute = new RORouteDef_Complete(myCurrentRouteName, myColor, *list, myTryRepair);
        }
        myColor = 0;
    }
    delete list;
}


void
RORDLoader_SUMOBase::myEndElement(SumoXMLTag element) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_ROUTE:
        if (!myAltIsValid) {
            return;
        }
        if (myCurrentRoute!=0&&myCurrentIsOk) {
            if (myCurrentAlternatives==0) {
                myNet.addRouteDef(myCurrentRoute);
                myCurrentRoute = 0;
            }
            if (myVehicleParameter==0) {
                myHaveNextRoute = true;
            }
            myCurrentRoute = 0;
        }
        break;
    case SUMO_TAG_ROUTE_DISTRIBUTION:
        if (!myCurrentIsOk) {
            return;
        }
        if (myVehicleParameter==0) {
            myHaveNextRoute = true;
        }
        myNet.addRouteDef(myCurrentAlternatives);
        myCurrentRoute = 0;
        myCurrentAlternatives = 0;
        break;
    case SUMO_TAG_VEHICLE:
        closeVehicle();
        delete myVehicleParameter;
        myVehicleParameter = 0;
        myHaveNextRoute = true;
        break;
    case SUMO_TAG_VTYPE: {
        SUMOVehicleParserHelper::closeVTypeParsing(*myCurrentVType);
        myNet.addVehicleType(myCurrentVType);
        myCurrentVType = 0;
    }
    default:
        break;
    }
    if (!myCurrentIsOk) {
        throw ProcessError();
    }
}


bool
RORDLoader_SUMOBase::closeVehicle() throw() {
    // get the vehicle id
    if (myVehicleParameter->depart<myBegin||myVehicleParameter->depart>=myEnd) {
        myCurrentIsOk = false;
        return false;
    }
    // get vehicle type
    SUMOVTypeParameter *type = myNet.getVehicleTypeSecure(myVehicleParameter->vtypeid);
    // get the route
    RORouteDef *route = myNet.getRouteDef(myVehicleParameter->routeid);
    if (route==0) {
        route = myNet.getRouteDef("!" + myVehicleParameter->id);
    }
    if (route==0) {
        MsgHandler::getErrorInstance()->inform("The route of the vehicle '" + myVehicleParameter->id + "' is not known.");
        myCurrentIsOk = false;
        return false;
    }
    // build the vehicle
    if (!MsgHandler::getErrorInstance()->wasInformed()) {
        ROVehicle *veh = new ROVehicle(*myVehicleParameter, route, type);
        myNet.addVehicle(myVehicleParameter->id, veh);
        return true;
    }
    return false;
}


void
RORDLoader_SUMOBase::beginNextRoute() throw() {
    myHaveNextRoute = false;
}



/****************************************************************************/

