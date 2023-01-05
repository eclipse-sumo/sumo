/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    NBPTLine.cpp
/// @author  Gregor Laemmel
/// @author  Nikita Cherednychek
/// @date    Tue, 20 Mar 2017
///
// The representation of one direction of a single pt line
/****************************************************************************/
#include <utils/iodevices/OutputDevice.h>

#include <utility>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include "NBEdge.h"
#include "NBEdgeCont.h"
#include "NBPTStop.h"
#include "NBPTStopCont.h"
#include "NBPTLine.h"


// ===========================================================================
// method definitions
// ===========================================================================
NBPTLine::NBPTLine(const std::string& id, const std::string& name, const std::string& type, const std::string& ref, int interval, const std::string& nightService,
                   SUMOVehicleClass vClass, RGBColor color) :
    myName(name),
    myType(type),
    myPTLineId(id),
    myRef(ref != "" ? ref : name),
    myColor(color),
    myInterval(interval),
    myNightService(nightService),
    myVClass(vClass)
{ }


void
NBPTLine::addPTStop(std::shared_ptr<NBPTStop> pStop) {
    if (!myPTStops.empty() && pStop->getName() != "" && myPTStops.back()->getName() == pStop->getName()) {
        // avoid duplicate stop when both platform and stop_position are given as nodes
        if (myPTStops.back()->isPlatform() && !pStop->isPlatform()) {
            myPTStops.pop_back();
        } else if (pStop->isPlatform()) {
            return;
        }
    }
    myPTStops.push_back(pStop);
}


const std::vector<std::shared_ptr<NBPTStop> >&
NBPTLine::getStops() {
    return myPTStops;
}


void
NBPTLine::write(OutputDevice& device) {
    device.openTag(SUMO_TAG_PT_LINE);
    device.writeAttr(SUMO_ATTR_ID, myPTLineId);
    if (!myName.empty()) {
        device.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(myName));
    }

    device.writeAttr(SUMO_ATTR_LINE, StringUtils::escapeXML(myRef));
    device.writeAttr(SUMO_ATTR_TYPE, myType);
    device.writeAttr(SUMO_ATTR_VCLASS, toString(myVClass));
    if (myInterval > 0) {
        // write seconds
        device.writeAttr(SUMO_ATTR_PERIOD, 60 * myInterval);
    }
    if (myNightService != "") {
        device.writeAttr("nightService", myNightService);
    }

    if (myColor.isValid()) {
        device.writeAttr(SUMO_ATTR_COLOR, myColor);
    }
    device.writeAttr("completeness", toString((double)myPTStops.size() / (double)myNumOfStops));

    if (!myRoute.empty()) {
        device.openTag(SUMO_TAG_ROUTE);
        device.writeAttr(SUMO_ATTR_EDGES, myRoute);
        device.closeTag();
    }

    for (auto& myPTStop : myPTStops) {
        device.openTag(SUMO_TAG_BUS_STOP);
        device.writeAttr(SUMO_ATTR_ID, myPTStop->getID());
        device.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(myPTStop->getName()));
        device.closeTag();
    }
    device.closeTag();

}


void
NBPTLine::addWayNode(long long int way, long long int node) {
    std::string wayStr = toString(way);
    if (wayStr != myCurrentWay) {
        myCurrentWay = wayStr;
        myWays.push_back(wayStr);
    }
    myWayNodes[wayStr].push_back(node);
}


const std::vector<long long int>*
NBPTLine::getWayNodes(std::string wayId) {
    if (myWayNodes.find(wayId) != myWayNodes.end()) {
        return &myWayNodes[wayId];
    }
    return nullptr;
}


void
NBPTLine::setEdges(const std::vector<NBEdge*>& edges) {
    myRoute = edges;
    // ensure permissions
    for (NBEdge* e : edges) {
        SVCPermissions permissions = e->getPermissions();
        if ((permissions & myVClass) != myVClass) {
            SVCPermissions nVuln = ~(SVC_PEDESTRIAN | SVC_BICYCLE);
            if (permissions != 0 && (permissions & nVuln) == 0) {
                // this is a footpath or sidewalk. Add another lane
                e->addRestrictedLane(SUMO_const_laneWidth, myVClass);
            } else {
                // add permissions to the rightmost lane that is not exclusively used for pedestrians / bicycles
                for (int i = 0; i < (int)e->getNumLanes(); i++) {
                    if ((e->getPermissions(i) & nVuln) != 0) {
                        e->allowVehicleClass(i, myVClass);
                        break;
                    }
                }
            }
        }
    }
}


void
NBPTLine::setMyNumOfStops(int numStops) {
    myNumOfStops = numStops;
}


const std::vector<NBEdge*>&
NBPTLine::getRoute() const {
    return myRoute;
}


std::vector<std::pair<NBEdge*, std::string> >
NBPTLine::getStopEdges(const NBEdgeCont& ec) const {
    std::vector<std::pair<NBEdge*, std::string> > result;
    for (std::shared_ptr<NBPTStop> stop : myPTStops) {
        NBEdge* e = ec.retrieve(stop->getEdgeId());
        if (e != nullptr) {
            result.push_back({e, stop->getID()});
        }
    }
    return result;
}


NBEdge*
NBPTLine::getRouteStart(const NBEdgeCont& ec) const {
    std::vector<NBEdge*> validEdges;
    // filter out edges that have been removed due to joining junctions
    for (NBEdge* e : myRoute) {
        if (ec.retrieve(e->getID())) {
            validEdges.push_back(e);
        }
    }
    if (validEdges.size() == 0) {
        return nullptr;
    }
    // filter out edges after the first stop
    if (myPTStops.size() > 0) {
        NBEdge* firstStopEdge = ec.retrieve(myPTStops.front()->getEdgeId());
        if (firstStopEdge == nullptr) {
            WRITE_WARNINGF(TL("Could not retrieve edge '%' for first stop of line '%'."), myPTStops.front()->getEdgeId(), myPTLineId);
            return nullptr;

        }
        auto it = std::find(validEdges.begin(), validEdges.end(), firstStopEdge);
        if (it == validEdges.end()) {
            WRITE_WARNINGF(TL("First stop edge '%' is not part of the route of line '%'."), firstStopEdge->getID(), myPTLineId);
            return nullptr;
        }
    }
    return validEdges.front();
}


NBEdge*
NBPTLine::getRouteEnd(const NBEdgeCont& ec) const {
    std::vector<NBEdge*> validEdges;
    // filter out edges that have been removed due to joining junctions
    for (NBEdge* e : myRoute) {
        if (ec.retrieve(e->getID())) {
            validEdges.push_back(e);
        }
    }
    if (validEdges.size() == 0) {
        return nullptr;
    }
    // filter out edges after the last stop
    if (myPTStops.size() > 0) {
        NBEdge* lastStopEdge = ec.retrieve(myPTStops.back()->getEdgeId());
        if (lastStopEdge == nullptr) {
            WRITE_WARNINGF(TL("Could not retrieve edge '%' for last stop of line '%'."), myPTStops.back()->getEdgeId(), myPTLineId);
            return nullptr;

        }
        auto it = std::find(validEdges.begin(), validEdges.end(), lastStopEdge);
        if (it == validEdges.end()) {
            WRITE_WARNINGF(TL("Last stop edge '%' is not part of the route of line '%'."), lastStopEdge->getID(), myPTLineId);
            return nullptr;
        }
    }
    return validEdges.back();
}


bool
NBPTLine::isConsistent(const std::vector<NBEdge*>& stops) const {
    if (myRoute.empty() || stops.empty()) {
        return true;
    }
    std::vector<NBEdge*>::const_iterator stopIt = stops.begin();
    for (const NBEdge* const e : myRoute) {
        while (stopIt != stops.end() && e == *stopIt) {
            ++stopIt;
        }
        if (stopIt == stops.end()) {
            return true;
        }
    }
    return false;
}


void
NBPTLine::replaceStop(std::shared_ptr<NBPTStop> oldStop, std::shared_ptr<NBPTStop> newStop) {
    for (int i = 0; i < (int)myPTStops.size(); i++) {
        if (myPTStops[i] == oldStop) {
            myPTStops[i] = newStop;
        }
    }
}


void
NBPTLine::replaceEdge(const std::string& edgeID, const EdgeVector& replacement) {
    EdgeVector oldRoute = myRoute;
    myRoute.clear();
    for (NBEdge* e : oldRoute) {
        if (e->getID() == edgeID) {
            for (NBEdge* e2 : replacement) {
                if (myRoute.empty() || myRoute.back() != e2) {
                    myRoute.push_back(e2);
                }
            }
        } else {
            myRoute.push_back(e);
        }
    }
}


void
NBPTLine::deleteInvalidStops(const NBEdgeCont& ec, const NBPTStopCont& sc) {
    // delete stops that are missing or have no edge
    for (auto it = myPTStops.begin(); it != myPTStops.end();) {
        std::shared_ptr<NBPTStop> stop = *it;
        if (sc.get(stop->getID()) == nullptr ||
                ec.getByID(stop->getEdgeId()) == nullptr) {
            WRITE_WARNINGF(TL("Removed invalid stop '%' from line '%'."), stop->getID(), getLineID());
            it = myPTStops.erase(it);
        } else {
            it++;
        }

    }
}


void
NBPTLine::deleteDuplicateStops() {
    // delete subsequent stops that belong to the same stopArea
    long long int lastAreaID = -1;
    std::string lastName = "";
    for (auto it = myPTStops.begin(); it != myPTStops.end();) {
        std::shared_ptr<NBPTStop> stop = *it;
        if (lastAreaID != -1 && stop->getAreaID() == lastAreaID) {
            WRITE_WARNINGF(TL("Removed duplicate stop '%' at area '%' from line '%'."), stop->getID(), toString(lastAreaID), getLineID());
            it = myPTStops.erase(it);
        } else if (lastName != "" && stop->getName() == lastName) {
            WRITE_WARNINGF(TL("Removed duplicate stop '%' named '%' from line '%'."), stop->getID(), lastName, getLineID());
            it = myPTStops.erase(it);
        } else {
            it++;
        }
        lastAreaID = stop->getAreaID();
        lastName = stop->getName();
    }
}


void
NBPTLine::removeInvalidEdges(const NBEdgeCont& ec) {
    for (int i = 0; i < (int)myRoute.size();) {
        const std::pair<NBEdge*, NBEdge*>* split = ec.getSplit(myRoute[i]);
        if (split != nullptr) {
            myRoute[i] = split->first;
            myRoute.insert(myRoute.begin() + i + 1, split->second);
        } else if (ec.retrieve(myRoute[i]->getID()) == nullptr) {
            myRoute.erase(myRoute.begin() + i);
        } else {
            i++;
        }
    }
}


/****************************************************************************/
