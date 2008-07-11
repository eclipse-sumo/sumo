/****************************************************************************/
/// @file    RORouteDef_Complete.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A complete route definition (with all passed edges being known)
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
#include <deque>
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include <utils/common/SUMOAbstractRouter.h>
#include <utils/common/UtilExceptions.h>
#include "RORouteDef_Complete.h"
#include "ROHelper.h"
#include <utils/iodevices/OutputDevice.h>

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
RORouteDef_Complete::RORouteDef_Complete(const std::string &id,
        const RGBColor &color,
        const std::vector<const ROEdge*> &edges) throw()
        : RORouteDef(id, color), myEdges(edges)
{
}


RORouteDef_Complete::~RORouteDef_Complete() throw()
{}


RORoute *
RORouteDef_Complete::buildCurrentRoute(SUMOAbstractRouter<ROEdge,ROVehicle> &router,
                                       SUMOTime begin, const ROVehicle &veh) const
{
    // !!!!
    if(false) {
        std::vector<const ROEdge*> newEdges;
        const std::vector<const ROEdge*> &oldEdges = myEdges;
        newEdges.push_back(*(oldEdges.begin()));
        for(std::vector<const ROEdge*>::const_iterator i=oldEdges.begin()+1; i!=oldEdges.end(); ++i) {
            if((*(i-1))->isConnectedTo(*i)) {
                newEdges.push_back(*i);
            } else {
                std::vector<const ROEdge*> edges;
                router.compute(*(i-1), *i, &veh, begin, edges);
                if(edges.size()==0) {
                    return 0;
                }
                std::copy(edges.begin()+1, edges.end(), back_inserter(newEdges));

            }
        }
        myEdges = newEdges;
    }
    SUMOReal costs = ROHelper::recomputeCosts(myEdges, &veh, begin);
    if(costs<0) {
        throw ProcessError("Route '" + getID() + "' (vehicle '" + veh.getID() + "') is not valid.");
    }
    return new RORoute(myID, 0, 1, myEdges, myColor);
}


void
RORouteDef_Complete::addAlternative(const ROVehicle *const, RORoute *current, SUMOTime begin)
{
    myStartTime = begin;
    delete current;
}


RORouteDef *
RORouteDef_Complete::copy(const std::string &id) const
{
    return new RORouteDef_Complete(id, myColor, myEdges);
}


OutputDevice &
RORouteDef_Complete::writeXMLDefinition(OutputDevice &dev, const ROVehicle * const veh, bool asAlternatives) const
{
    // (optional) alternatives header
    if(asAlternatives) {
        dev << "      <routealt last=\"1\"";
        if (myColor!=RGBColor()) {
             dev << " color=\"" << myColor << "\"";
        }
        dev << ">\n   ";
    }
    // the route
    dev << "      <route";
    if (myColor!=RGBColor()) {
        dev << " color=\"" << myColor << "\"";
    }
    if(asAlternatives) {
        dev << " cost=\"" << ROHelper::recomputeCosts(myEdges, veh, veh->getDepartureTime());
        dev << "\" probability=\"1";
    }
    dev << ">" << myEdges << "</route>\n";
    // (optional) alternatives end
    if(asAlternatives) {
        dev << "      </routealt>\n";
    }
    return dev;
}



/****************************************************************************/

