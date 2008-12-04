/****************************************************************************/
/// @file    MSRouteProbe.cpp
/// @author  Michael Behrisch
/// @date    Thu, 04.12.2008
/// @version $Id$
///
// Writes route distributions at a certain edge
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
#include <microsim/MSEdge.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSRoute.h>
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSRouteProbe::MSRouteProbe(const string &id,
                           const MSEdge *edge) throw()
        : Named(id)
{
    myCurrentRouteDistribution = new RandomDistributor<const MSRoute*>();
#ifdef HAVE_MESOSIM
    MSGlobals::gMesoNet->getSegmentForEdge(edge)->setRouteProbe(this);
#endif
}


MSRouteProbe::~MSRouteProbe() throw()
{
}


void
MSRouteProbe::writeXMLOutput(OutputDevice &dev,
                             SUMOTime startTime, SUMOTime) throw(IOError)
{
    if (myCurrentRouteDistribution->getOverallProb() > 0) {
        const std::string indent("    ");
        const std::string id = getID() + "_" + toString(startTime);
        dev << indent << "<routeDistribution id=\"" << id << "\">\n";
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
        MSRoute::dictionary(id, myCurrentRouteDistribution);
        myCurrentRouteDistribution = new RandomDistributor<const MSRoute*>();
    }
}


void
MSRouteProbe::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError)
{
    dev.writeXMLHeader("route-probes");
}


void
MSRouteProbe::addRoute(const MSRoute &route) const
{
    if (myCurrentRouteDistribution != 0) {
        myCurrentRouteDistribution->add(1., &route);
    }
}


