/****************************************************************************/
/// @file    MSRouteProbe.cpp
/// @author  Michael Behrisch
/// @date    Thu, 04.12.2008
/// @version $Id$
///
// Writes route distributions at a certain edge
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

#include <string>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSRoute.h>
#include <microsim/MSVehicle.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#endif
#include "MSRouteProbe.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSRouteProbe::EntryReminder::EntryReminder(MSLane * const lane, MSRouteProbe& collector) throw()
        : MSMoveReminder(lane), myCollector(collector) {}


bool
MSRouteProbe::EntryReminder::isStillActive(MSVehicle& veh, SUMOReal oldPos,
        SUMOReal newPos, SUMOReal newSpeed) throw() {
    myCollector.addRoute(veh.getRoute());
    return false;
}


bool
MSRouteProbe::EntryReminder::isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw() {
    if (isEmit) {
        myCollector.addRoute(veh.getRoute());
    }
    return false;
}



MSRouteProbe::MSRouteProbe(const std::string &id, const MSEdge *edge, SUMOTime begin) throw()
        : Named(id), myCurrentRouteDistribution(0) {
    const std::string distID = id + "_" + toString(begin);
    myCurrentRouteDistribution = MSRoute::distDictionary(distID);
    if (myCurrentRouteDistribution == 0) {
        myCurrentRouteDistribution = new RandomDistributor<const MSRoute*>();
        MSRoute::dictionary(distID, myCurrentRouteDistribution);
    }
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MESegment *seg = MSGlobals::gMesoNet->getSegmentForEdge(edge);
        while (seg!=0) {
            seg->setRouteProbe(this);
            seg = seg->getNextSegment();
        }
        return;
    }
#endif
    MSEdge::LaneCont::const_iterator it = edge->getLanes()->begin();
    myEntryReminder = new MSRouteProbe::EntryReminder(*it, *this);
    for (++it; it!=edge->getLanes()->end(); ++it) {
        (*it)->addMoveReminder(myEntryReminder);
    }
}


MSRouteProbe::~MSRouteProbe() throw() {
}


void
MSRouteProbe::writeXMLOutput(OutputDevice &dev,
                             SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    if (myCurrentRouteDistribution->getOverallProb() > 0) {
        const std::string indent("    ");
        dev << indent << "<routeDistribution id=\"" << getID() + "_" + toString(startTime) << "\">\n";
        const std::vector<const MSRoute*> &routes = myCurrentRouteDistribution->getVals();
        const std::vector<SUMOReal> &probs = myCurrentRouteDistribution->getProbs();
        for (unsigned int j=0; j<routes.size(); ++j) {
            const MSRoute *r = routes[j];
            dev << indent << indent << "<route id=\"" << r->getID() << "_" << startTime << "\" edges=\"";
            MSRouteIterator i = r->begin();
            for (; i!=r->end(); ++i) {
                const MSEdge *e = *i;
                dev << e->getID() << " ";
            }
            dev << "\" probability=\"" << probs[j] << "\"/>\n";
        }
        dev << indent << "</routeDistribution>\n";
        myCurrentRouteDistribution = new RandomDistributor<const MSRoute*>();
        MSRoute::dictionary(getID() + "_" + toString(stopTime), myCurrentRouteDistribution);
    }
}


void
MSRouteProbe::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError) {
    dev.writeXMLHeader("route-probes");
}


void
MSRouteProbe::addRoute(const MSRoute &route) const {
    if (myCurrentRouteDistribution != 0) {
        const MSRoute* routep = &route;
        if (!route.inFurtherUse()) {
            const std::string id = getID() + "_" + route.getID();
            routep = new MSRoute(id, route.getEdges(), true);
            MSRoute::dictionary(id, routep);
        }
        myCurrentRouteDistribution->add(1., routep);
    }
}


