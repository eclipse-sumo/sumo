/****************************************************************************/
/// @file    MSRouteProbe.cpp
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Tino Morenz
/// @date    Thu, 04.12.2008
/// @version $Id$
///
// Writes route distributions at a certain edge
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

#include <string>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSRoute.h>
#include <microsim/MSVehicle.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#ifdef HAVE_INTERNAL
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
MSRouteProbe::MSRouteProbe(const std::string& id, const MSEdge* edge, SUMOTime begin) :
    MSDetectorFileOutput(id),
    MSMoveReminder(id),
    myCurrentRouteDistribution(0) {
    const std::string distID = id + "_" + toString(begin);
    myCurrentRouteDistribution = MSRoute::distDictionary(distID);
    if (myCurrentRouteDistribution == 0) {
        myCurrentRouteDistribution = new RandomDistributor<const MSRoute*>(MSRoute::getMaxRouteDistSize(), &MSRoute::releaseRoute);
        MSRoute::dictionary(distID, myCurrentRouteDistribution);
    }
#ifdef HAVE_INTERNAL
    if (MSGlobals::gUseMesoSim) {
        MESegment* seg = MSGlobals::gMesoNet->getSegmentForEdge(*edge);
        while (seg != 0) {
            seg->addDetector(this);
            seg = seg->getNextSegment();
        }
        return;
    }
#endif
    for (std::vector<MSLane*>::const_iterator it = edge->getLanes().begin(); it != edge->getLanes().end(); ++it) {
        (*it)->addMoveReminder(this);
    }
}


MSRouteProbe::~MSRouteProbe() {
}


bool
MSRouteProbe::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason) {
    if (myCurrentRouteDistribution != 0 && reason != MSMoveReminder::NOTIFICATION_SEGMENT && reason != MSMoveReminder::NOTIFICATION_LANE_CHANGE) {
        veh.getRoute().addReference();
        myCurrentRouteDistribution->add(1., &veh.getRoute());
    }
    return false;
}


void
MSRouteProbe::writeXMLOutput(OutputDevice& dev,
                             SUMOTime startTime, SUMOTime stopTime) {
    if (myCurrentRouteDistribution->getOverallProb() > 0) {
        dev.openTag("routeDistribution") << " id=\"" << getID() + "_" + time2string(startTime) << "\"";
        const std::vector<const MSRoute*>& routes = myCurrentRouteDistribution->getVals();
        const std::vector<SUMOReal>& probs = myCurrentRouteDistribution->getProbs();
        for (unsigned int j = 0; j < routes.size(); ++j) {
            const MSRoute* r = routes[j];
            dev.openTag("route") << " id=\"" << r->getID() + "_" + time2string(startTime) << "\" edges=\"";
            for (MSRouteIterator i = r->begin(); i != r->end(); ++i) {
                if (i != r->begin()) {
                    dev << " ";
                }
                dev << (*i)->getID();
            }
            dev << "\" probability=\"" << probs[j] << "\"";
            dev.closeTag();
        }
        dev.closeTag();
        myCurrentRouteDistribution = new RandomDistributor<const MSRoute*>(MSRoute::getMaxRouteDistSize(), &MSRoute::releaseRoute);
        MSRoute::dictionary(getID() + "_" + toString(stopTime), myCurrentRouteDistribution);
    }
}


void
MSRouteProbe::writeXMLDetectorProlog(OutputDevice& dev) const {
    dev.writeXMLHeader("route-probes");
}
