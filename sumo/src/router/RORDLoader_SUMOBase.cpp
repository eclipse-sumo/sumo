/****************************************************************************/
/// @file    RORDLoader_SUMOBase.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The base class for SUMO-native route handlers
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

#include "RORDLoader_SUMOBase.h"
#include "ROVehicleType.h"
#include "RORouteDef.h"
#include "RONet.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include "ROVehicleType_Krauss.h"
#include "ROVehicleBuilder.h"
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
RORDLoader_SUMOBase::RORDLoader_SUMOBase(ROVehicleBuilder &vb, RONet &net,
        SUMOTime begin, SUMOTime end,
        SUMOReal gawronBeta, SUMOReal gawronA,
        int maxRouteNumber, bool tryRepair,
        const std::string &file) throw(ProcessError)
        : ROTypedXMLRoutesLoader(vb, net, begin, end, file),
        myVehicleParameter(0), myCurrentIsOk(true), myAltIsValid(true), myHaveNextRoute(false),
        myCurrentAlternatives(0),
        myGawronBeta(gawronBeta), myGawronA(gawronA), myMaxRouteNumber(maxRouteNumber),
        myCurrentRoute(0), myCurrentDepart(-1), myTryRepair(tryRepair), myColor(0)
{
}


RORDLoader_SUMOBase::~RORDLoader_SUMOBase() throw()
{
    // clean up (on failure)
    delete myCurrentAlternatives;
    delete myCurrentRoute;
    delete myVehicleParameter;
    delete myColor;
}


void
RORDLoader_SUMOBase::myStartElement(SumoXMLTag element,
                                    const SUMOSAXAttributes &attrs) throw(ProcessError)
{
    switch (element) {
    case SUMO_TAG_ROUTE:
        startRoute(attrs);
        break;
    case SUMO_TAG_VEHICLE:
        // try to parse the vehicle definition
        delete myVehicleParameter;
        myVehicleParameter = 0;
        myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs);
        if(myVehicleParameter!=0) {
            myCurrentDepart = myVehicleParameter->depart;
        }
        myCurrentIsOk = myVehicleParameter!=0;
        break;
    case SUMO_TAG_VTYPE:
        startVehType(attrs);
        break;
    case SUMO_TAG_ROUTEALT:
        myAltIsValid = true;
        startAlternative(attrs);
        if(!myCurrentIsOk) {
            myAltIsValid = false;
        }
        break;
    default:
        break;
    }
    if(!myCurrentIsOk) {
        throw ProcessError();
    }
}


void
RORDLoader_SUMOBase::startRoute(const SUMOSAXAttributes &attrs)
{
    delete myColor;
    myColor = 0;
    if(!myAltIsValid) {
        return;
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
            if(attrs.hasAttribute(SUMO_ATTR_COLOR)) {
                myColor = new RGBColor(RGBColor::parseColor(attrs.getString(SUMO_ATTR_COLOR)));
            }
        } catch (EmptyData &) {
            myCurrentRouteName = "";
            MsgHandler::getErrorInstance()->inform("Missing id in route.");
            myCurrentIsOk = false;
        }
        return;
    }
    // parse route alternative...
    myCost = attrs.getSUMORealReporting(SUMO_ATTR_COST, "route(alternative)", myCurrentAlternatives->getID().c_str(), myCurrentIsOk);
    myProbability = attrs.getSUMORealReporting(SUMO_ATTR_PROB, "route(alternative)", myCurrentAlternatives->getID().c_str(), myCurrentIsOk);
    if(attrs.hasAttribute(SUMO_ATTR_COLOR)) {
        myColor = new RGBColor(RGBColor::parseColor(attrs.getString(SUMO_ATTR_COLOR)));
    }
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


void
RORDLoader_SUMOBase::startAlternative(const SUMOSAXAttributes &attrs)
{
    // try to get the id
    myCurrentIsOk = true;
    string id;
    if (myVehicleParameter!=0) {
        id = myVehicleParameter->id;
        if(id=="") {
            MsgHandler::getErrorInstance()->inform("Missing 'id' of a routealt.");
            myCurrentIsOk = false;
            return;    
        }
        id = "!" + id;
    } else {
        if(!attrs.setIDFromAttributes("routealt", id)) {
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
                                  const std::string &chars) throw(ProcessError)
{
    // process routes only, all other elements do
    //  not have embedded characters
    if (element!=SUMO_TAG_ROUTE) {
        return;
    }
    if(!myAltIsValid) {
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
            if(false) {
                MsgHandler::getErrorInstance()->inform("The route '" + myCurrentAlternatives->getID() + "' contains the unknown edge '" + id + "'.");
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
RORDLoader_SUMOBase::myEndElement(SumoXMLTag element) throw(ProcessError)
{
    switch (element) {
    case SUMO_TAG_ROUTE:
        if(!myAltIsValid) {
            return;
        }
        if (myCurrentRoute!=0&&myCurrentIsOk) {
            if(myCurrentAlternatives==0) {
                myNet.addRouteDef(myCurrentRoute);
                myCurrentRoute = 0;
            }
            if (myVehicleParameter==0) {
                myHaveNextRoute = true;
            }
            myCurrentRoute = 0;
        }
        break;
    case SUMO_TAG_ROUTEALT:
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
    default:
        break;
    }
    if(!myCurrentIsOk) {
        throw ProcessError();
    }
}


bool
RORDLoader_SUMOBase::closeVehicle() throw()
{
    // get the vehicle id
    if (myVehicleParameter->depart<myBegin||myVehicleParameter->depart>=myEnd) {
        myCurrentIsOk = false;
        return false;
    }
    // get vehicle type
    ROVehicleType *type = myNet.getVehicleTypeSecure(myVehicleParameter->vtypeid);
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
        ROVehicle *veh = myVehicleBuilder.buildVehicle(*myVehicleParameter, route, type);
        myNet.addVehicle(myVehicleParameter->id, veh);
        return true;
    }
    return false;
}


void
RORDLoader_SUMOBase::startVehType(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttributes("vtype", id, false)) {
        MsgHandler::getErrorInstance()->inform("Missing id in vtype.");
        myCurrentIsOk = false;
        return;
    }
    // get the other values
    try {
        SUMOReal maxspeed = attrs.getFloatSecure(SUMO_ATTR_MAXSPEED, DEFAULT_VEH_MAXSPEED);
        SUMOReal length = attrs.getFloatSecure(SUMO_ATTR_LENGTH, DEFAULT_VEH_LENGTH);
        SUMOReal accel = attrs.getFloatSecure(SUMO_ATTR_ACCEL, DEFAULT_VEH_A);
        SUMOReal decel = attrs.getFloatSecure(SUMO_ATTR_DECEL, DEFAULT_VEH_B);
        SUMOReal sigma = attrs.getFloatSecure(SUMO_ATTR_SIGMA, DEFAULT_VEH_SIGMA);
        SUMOReal tau = attrs.getFloatSecure(SUMO_ATTR_TAU, DEFAULT_VEH_TAU);
        std::string color = attrs.getStringSecure(SUMO_ATTR_COLOR, "");
        SUMOVehicleClass vclass = SUMOVehicleParserHelper::parseVehicleClass(attrs, "vtype", id);
        // build the vehicle type
        //  by now, only vehicles using the krauss model are supported
        ROVehicleType *vtype = new ROVehicleType_Krauss(id, color, length, vclass, accel, decel, sigma, maxspeed, tau);
        myNet.addVehicleType(vtype);
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("At least one parameter of vehicle type '" + id + "' is not numeric, but should be.");
    }
}


void
RORDLoader_SUMOBase::beginNextRoute() throw()
{
    myHaveNextRoute = false;
}



/****************************************************************************/

