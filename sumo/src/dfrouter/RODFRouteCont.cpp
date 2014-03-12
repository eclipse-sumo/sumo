/****************************************************************************/
/// @file    RODFRouteCont.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A container for routes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include <fstream>
#include <cassert>
#include "RODFRouteDesc.h"
#include "RODFRouteCont.h"
#include "RODFNet.h"
#include <router/ROEdge.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RODFRouteCont::RODFRouteCont() {}


RODFRouteCont::~RODFRouteCont() {
}


void
RODFRouteCont::addRouteDesc(RODFRouteDesc& desc) {
    // routes may be duplicate as in-between routes may have different starting points
    if (find_if(myRoutes.begin(), myRoutes.end(), route_finder(desc)) == myRoutes.end()) {
        // compute route id
        setID(desc);
        myRoutes.push_back(desc);
    } else {
        RODFRouteDesc& prev = *find_if(myRoutes.begin(), myRoutes.end(), route_finder(desc));
        prev.overallProb += desc.overallProb;
    }
}


bool
RODFRouteCont::removeRouteDesc(RODFRouteDesc& desc) {
    std::vector<RODFRouteDesc>::const_iterator j = find_if(myRoutes.begin(), myRoutes.end(), route_finder(desc));
    if (j == myRoutes.end()) {
        return false;
    }
    return true;
}


bool
RODFRouteCont::save(std::vector<std::string>& saved,
                    const std::string& prependix, OutputDevice& out) {
    bool haveSavedOneAtLeast = false;
    for (std::vector<RODFRouteDesc>::const_iterator j = myRoutes.begin(); j != myRoutes.end(); ++j) {
        const RODFRouteDesc& desc = (*j);
        if (find(saved.begin(), saved.end(), desc.routename) != saved.end()) {
            continue;
        }
        saved.push_back((*j).routename);
        assert(desc.edges2Pass.size() >= 1);
        out.openTag(SUMO_TAG_ROUTE).writeAttr(SUMO_ATTR_ID, prependix + desc.routename);
        out << " edges=\"";
        for (std::vector<ROEdge*>::const_iterator k = desc.edges2Pass.begin(); k != desc.edges2Pass.end(); k++) {
            if (k != desc.edges2Pass.begin()) {
                out << ' ';
            }
            out << (*k)->getID();
        }
        out << '"';
        out.closeTag();
        haveSavedOneAtLeast = true;
    }
    return haveSavedOneAtLeast;
}


void
RODFRouteCont::sortByDistance() {
    sort(myRoutes.begin(), myRoutes.end(), by_distance_sorter());
}


void
RODFRouteCont::removeIllegal(const std::vector<std::vector<ROEdge*> >& illegals) {
    for (std::vector<RODFRouteDesc>::iterator i = myRoutes.begin(); i != myRoutes.end();) {
        RODFRouteDesc& desc = *i;
        bool remove = false;
        for (std::vector<std::vector<ROEdge*> >::const_iterator j = illegals.begin(); !remove && j != illegals.end(); ++j) {
            int noFound = 0;
            for (std::vector<ROEdge*>::const_iterator k = (*j).begin(); !remove && k != (*j).end(); ++k) {
                if (find(desc.edges2Pass.begin(), desc.edges2Pass.end(), *k) != desc.edges2Pass.end()) {
                    noFound++;
                    if (noFound > 1) {
                        remove = true;
                    }
                }
            }
        }
        if (remove) {
            i = myRoutes.erase(i);
        } else {
            ++i;
        }
    }
}


void
RODFRouteCont::setID(RODFRouteDesc& desc) const {
    std::pair<ROEdge*, ROEdge*> c(desc.edges2Pass[0], desc.edges2Pass.back());
    desc.routename = c.first->getID() + "_to_" + c.second->getID();
    if (myConnectionOccurences.find(c) == myConnectionOccurences.end()) {
        myConnectionOccurences[c] = 0;
    } else {
        myConnectionOccurences[c] = myConnectionOccurences[c] + 1;
        desc.routename = desc.routename + "_" + toString(myConnectionOccurences[c]);
    }
}



/****************************************************************************/

