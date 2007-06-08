/****************************************************************************/
/// @file    RORDLoader_SUMOAlt.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A SAX-handler for SUMO-route-alternatives
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
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gfx/GfxConvHelper.h>
#include <utils/common/ToString.h>
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "RORouteDef_Alternatives.h"
#include "ROVehicleType.h"
#include "RORDLoader_SUMOAlt.h"
#include "ROEdgeVector.h"
#include "RORoute.h"
#include "RONet.h"

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
RORDLoader_SUMOAlt::RORDLoader_SUMOAlt(ROVehicleBuilder &vb, RONet &net,
                                       SUMOTime begin, SUMOTime end,
                                       SUMOReal gawronBeta, SUMOReal gawronA,
                                       int maxRouteNumber,
                                       const std::string &file)
        : RORDLoader_SUMOBase(vb, net, begin, end,
                              "precomputed sumo route alternatives", file),
        myCurrentAlternatives(0),
        myGawronBeta(gawronBeta), myGawronA(gawronA), myMaxRouteNumber(maxRouteNumber)
{}


RORDLoader_SUMOAlt::~RORDLoader_SUMOAlt()
{}


void
RORDLoader_SUMOAlt::myStartElement(SumoXMLTag element,
                                   const std::string &name,
                                   const Attributes &attrs) throw()
{
    RORDLoader_SUMOBase::myStartElement(element, name, attrs);
    switch (element) {
    case SUMO_TAG_ROUTEALT:
        startAlternative(attrs);
        break;
    default:
        break;
    }
}


void
RORDLoader_SUMOAlt::startRoute(const Attributes &attrs)
{
    mySkipCurrent = false;
    if (myCurrentAlternatives==0) {
        getErrorHandlerMarkInvalid()->inform(
            "Route declaration without an alternatives container occured.");
        mySkipCurrent = true;
        return;
    }
    // try to get the costs
    try {
        myCost = getFloat(attrs, SUMO_ATTR_COST);
    } catch (NumberFormatException &) {
        getErrorHandlerMarkInvalid()->inform(
            "Invalid cost in alternative for route '" + myCurrentAlternatives->getID() + "' (" + getString(attrs, SUMO_ATTR_COST) + ").");
        mySkipCurrent = true;
        return;
    } catch (EmptyData &) {
        getErrorHandlerMarkInvalid()->inform("Missing cost in alternative for route '" + myCurrentAlternatives->getID() + "'.");
        mySkipCurrent = true;
        return;
    }
    if (myCost<0) {
        getErrorHandlerMarkInvalid()->inform("Invalid cost in alternative for route '" + myCurrentAlternatives->getID() + "' (" + toString<SUMOReal>(myCost) + ").");
        mySkipCurrent = true;
        return;
    }
    // try to get the probability
    try {
        myProbability = getFloatSecure(attrs, SUMO_ATTR_PROB, -10000);
    } catch (NumberFormatException &) {
        getErrorHandlerMarkInvalid()->inform("Invalid probability in alternative for route '" + myCurrentAlternatives->getID() + "' (" + toString<SUMOReal>(myProbability) + ").");
        mySkipCurrent = true;
        return;
    } catch (EmptyData &) {
        getErrorHandlerMarkInvalid()->inform("Missing probability in alternative for route '" + myCurrentAlternatives->getID() + "'.");
        mySkipCurrent = true;
        return;
    }
    if (myProbability<0) {
        getErrorHandlerMarkInvalid()->inform("Invalid probability in alternative for route '" + myCurrentAlternatives->getID() + "' (" + toString<SUMOReal>(myProbability) + ").");
        mySkipCurrent = true;
        return;
    }
}


void
RORDLoader_SUMOAlt::myCharacters(SumoXMLTag element, const std::string &name,
                                 const std::string &chars) throw()
{
    // process routes only, all other elements do
    //  not have embedded characters
    if (element!=SUMO_TAG_ROUTE) {
        return;
    }
    // check whether the costs and the probability are valid
    if (myCost<0||myProbability<0||mySkipCurrent) {
        return;
    }
    // build the list of edges
    ROEdgeVector *list = new ROEdgeVector();
    bool ok = true;
    StringTokenizer st(chars);
    while (ok&&st.hasNext()) { // !!! too slow !!!
        string id = st.next();
        ROEdge *edge = _net.getEdge(id);
        if (edge!=0) {
            list->add(edge);
        } else {
            getErrorHandlerMarkInvalid()->inform("The route '" + myCurrentAlternatives->getID() + "' contains the unknown edge '" + id + "'.");
            ok = false;
        }
    }
    if (ok) {
        myCurrentAlternatives->addLoadedAlternative(
            new RORoute(myCurrentAlternatives->getID(), myCost, myProbability, *list));
    }
    delete list;
}


void
RORDLoader_SUMOAlt::myEndElement(SumoXMLTag element, const std::string &name) throw()
{
    RORDLoader_SUMOBase::myEndElement(element, name);
    switch (element) {
    case SUMO_TAG_ROUTEALT:
        if (mySkipCurrent) {
            return;
        }
        if (!myAmInEmbeddedMode) {
            myHaveNextRoute = true;
        }
        endAlternative();
        break;
    case SUMO_TAG_VEHICLE:
        myHaveNextRoute = true;
        break;
    default:
        break;
    }
}


void
RORDLoader_SUMOAlt::startAlternative(const Attributes &attrs)
{
    // try to get the id
    string id;
    try {
        mySkipCurrent = false;
        if (myAmInEmbeddedMode) {
            id = getStringSecure(attrs, SUMO_ATTR_ID, "!" + myActiveVehicleID);
        } else {
            id = getString(attrs, SUMO_ATTR_ID);
        }
    } catch (EmptyData &) {
        getErrorHandlerMarkInvalid()->inform("Missing route alternative name.");
        return;
    }
    // try to get the index of the last element
    int index = getIntSecure(attrs, SUMO_ATTR_LAST, -1);
    if (index<0) {
        getErrorHandlerMarkInvalid()->inform("Missing or non-numeric index of a route alternative (id='" + id + "'.");
        return;
    }
    // try to get the color
    myCurrentColor = parseColor(*this, attrs, "route", myCurrentRouteName);
    // build the alternative cont
    myCurrentAlternatives = new RORouteDef_Alternatives(id, myCurrentColor,
                            index, myGawronBeta, myGawronA, myMaxRouteNumber);
}


void
RORDLoader_SUMOAlt::endAlternative()
{
    _net.addRouteDef(myCurrentAlternatives);
    myCurrentAlternatives = 0;
}



/****************************************************************************/

